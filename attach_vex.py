"""Attach OpenVEX documents to a pushed multi-platform image on Docker Hub.

    python3 attach_vex.py startown/madgraph:3.5.17-1 trixie-dev.vex.json

For each platform, every VEX document becomes an in-toto statement in that
platform's attestation manifest, next to the SBOM and provenance that Bake
wrote, and the tag moves to an index that points at the new attestation
manifests. The images themselves don't change.

Docker Scout doesn't carry a base image's VEX over to images built on it, so
attach the Docker Hardened Images VEX for every DHI image our packages come
from, exported with
`docker scout vex get --verify --skip-tlog --output <file> registry://<image>`.
Each document keeps its own author: Scout trusts `dhi@docker.com` by default.

`docker scout attestation add` does the same, but (Scout CLI 1.29) writes the
attestation manifests without `subject` and with a non-empty config, and their
index entries without a platform. Docker Scout then ignores every attestation
of the image (SBOM, provenance and VEX) and scans the image itself instead.
This script writes the attestations the way Bake does.

Running it again replaces the image's previous VEX documents. It signs in with
the Docker Hub login stored by Docker Desktop (docker-credential-desktop) and
never prints it.
"""

import base64
import hashlib
import json
import subprocess
import sys
import urllib.error
import urllib.parse
import urllib.request

OCI_INDEX = "application/vnd.oci.image.index.v1+json"
OCI_MANIFEST = "application/vnd.oci.image.manifest.v1+json"
ATTESTATION = "application/vnd.docker.attestation.manifest.v1+json"
IN_TOTO = "application/vnd.in-toto+json"
OPENVEX = "https://openvex.dev/ns/v0.2.0"
EMPTY_JSON = b"{}"
EMPTY = {
    "mediaType": "application/vnd.oci.empty.v1+json",
    "digest": "sha256:" + hashlib.sha256(EMPTY_JSON).hexdigest(),
    "size": len(EMPTY_JSON),
}


class Registry:
    def __init__(self, repo):
        self.repo = repo
        self.token = self._token()

    def _token(self):
        output = subprocess.run(
            ["docker-credential-desktop", "get"],
            input=b"https://index.docker.io/v1/",
            capture_output=True,
            check=True,
        ).stdout
        cred = json.loads(output)
        basic = base64.b64encode(f"{cred['Username']}:{cred['Secret']}".encode())
        url = (
            "https://auth.docker.io/token?service=registry.docker.io"
            f"&scope=repository:{self.repo}:pull,push"
        )
        request = urllib.request.Request(
            url=url,
            headers={"Authorization": "Basic " + basic.decode()},
        )
        with urllib.request.urlopen(request) as response:
            return json.load(response)["token"]

    def call(self, method, path, data=None, content_type=None, accept="*/*"):
        headers = {"Authorization": "Bearer " + self.token, "Accept": accept}
        if content_type:
            headers["Content-Type"] = content_type
        url = path if path.startswith("https://") else self.url(path)
        request = urllib.request.Request(url, data=data, method=method, headers=headers)
        with urllib.request.urlopen(request) as response:
            return response.status, response.headers, response.read()

    def url(self, path):
        return f"https://registry-1.docker.io/v2/{self.repo}/{path}"

    def manifest(self, ref, media_type):
        _, _, body = self.call("GET", "manifests/" + ref, accept=media_type)
        return json.loads(body)

    def put_manifest(self, manifest, ref=None):
        """Push a manifest under ref, or under its own digest. Returns the
        digest and the size of what was pushed."""
        body = json.dumps(manifest, separators=(",", ":")).encode()
        digest = "sha256:" + hashlib.sha256(body).hexdigest()
        self.call(
            method="PUT",
            path="manifests/" + (ref or digest),
            data=body,
            content_type=manifest["mediaType"],
        )
        return digest, len(body)

    def has_blob(self, digest):
        try:
            self.call("HEAD", "blobs/" + digest)
            return True
        except urllib.error.HTTPError as error:
            if error.code == 404:
                return False
            raise

    def put_blob(self, data):
        digest = "sha256:" + hashlib.sha256(data).hexdigest()
        if not self.has_blob(digest):
            _, headers, _ = self.call("POST", "blobs/uploads/")
            location = urllib.parse.urljoin(self.url(""), headers["Location"])
            separator = "&" if "?" in location else "?"
            self.call(
                method="PUT",
                path=f"{location}{separator}digest={digest}",
                data=data,
                content_type="application/octet-stream",
            )
        return digest


def predicate_type(layer):
    return (layer.get("annotations") or {}).get("in-toto.io/predicate-type")


def platform_name(platform):
    name = f"{platform['os']}/{platform['architecture']}"
    if platform.get("variant"):
        name += "/" + platform["variant"]
    return name


def main(image, vex_paths):
    repo, tag = image.rsplit(":", 1)
    documents = []
    for path in vex_paths:
        with open(path) as fsock:
            documents.append(json.load(fsock))
    registry = Registry(repo)
    registry.put_blob(EMPTY_JSON)

    index = registry.manifest(tag, OCI_INDEX)
    images = {
        m["digest"]: m
        for m in index["manifests"]
        if (m.get("platform") or {}).get("architecture") not in (None, "unknown")
    }
    for entry in index["manifests"]:
        annotations = entry.get("annotations") or {}
        if annotations.get("vnd.docker.reference.type") != "attestation-manifest":
            continue
        target = images[annotations["vnd.docker.reference.digest"]]
        old = registry.manifest(entry["digest"], OCI_MANIFEST)

        name = urllib.parse.quote(platform_name(target["platform"]), safe="")
        subject = {
            "name": f"pkg:docker/{repo}@{tag}?platform={name}",
            "digest": {"sha256": target["digest"].removeprefix("sha256:")},
        }
        layers = [d for d in old["layers"] if predicate_type(d) != OPENVEX]
        for vex in documents:
            statement = {
                "_type": "https://in-toto.io/Statement/v0.1",
                "predicateType": OPENVEX,
                "subject": [subject],
                "predicate": vex,
            }
            data = json.dumps(statement, separators=(",", ":")).encode()
            layers.append(
                {
                    "mediaType": IN_TOTO,
                    "digest": registry.put_blob(data),
                    "size": len(data),
                    "annotations": {"in-toto.io/predicate-type": OPENVEX},
                }
            )
        new = {
            "schemaVersion": 2,
            "mediaType": OCI_MANIFEST,
            "artifactType": old.get("artifactType", ATTESTATION),
            "config": EMPTY,
            "layers": layers,
            "subject": {
                "mediaType": target["mediaType"],
                "digest": target["digest"],
                "size": target["size"],
            },
        }
        digest, size = registry.put_manifest(new)
        entry["digest"], entry["size"] = digest, size
        entry["platform"] = {"architecture": "unknown", "os": "unknown"}
        print(f"{platform_name(target['platform'])}: attestations {digest}")

    digest, _ = registry.put_manifest(index, ref=tag)
    print(f"{image}: index {digest}")


if __name__ == "__main__":
    if len(sys.argv) < 3:
        sys.exit(__doc__.split("\n\n")[1])
    main(sys.argv[1], sys.argv[2:])
