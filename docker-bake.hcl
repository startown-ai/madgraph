target "fastjet" {
  platforms = ["linux/amd64", "linux/arm64"]
  context   = "fastjet"
  tags      = ["startown/fastjet:3.5.3-1"]
  attest    = ["type=provenance,mode=max", "type=sbom"]
}

target "hepmc" {
  platforms = ["linux/amd64", "linux/arm64"]
  context   = "hepmc"
  tags      = ["startown/hepmc:3.3.1-1"]
  attest    = ["type=provenance,mode=max", "type=sbom"]
}

target "lhapdf" {
  platforms = ["linux/amd64", "linux/arm64"]
  context   = "lhapdf"
  tags      = ["startown/lhapdf:6.5.6-2"]
  attest    = ["type=provenance,mode=max", "type=sbom"]
}

target "root" {
  platforms = ["linux/amd64", "linux/arm64"]
  context   = "root"
  tags      = ["startown/root:6.36.14-1"]
  attest    = ["type=provenance,mode=max", "type=sbom"]
}

target "delphes" {
  platforms = ["linux/amd64", "linux/arm64"]
  context   = "delphes"
  tags      = ["startown/delphes:3.5.1-1"]
  attest    = ["type=provenance,mode=max", "type=sbom"]
}

target "emela" {
  platforms = ["linux/amd64", "linux/arm64"]
  context   = "emela"
  tags      = ["startown/emela:1.0.1-1"]
  attest    = ["type=provenance,mode=max", "type=sbom"]
}

target "pythia" {
  platforms = ["linux/amd64", "linux/arm64"]
  context   = "pythia"
  tags      = ["startown/pythia:8.317-2"]
  attest    = ["type=provenance,mode=max", "type=sbom"]
}

target "madgraph" {
  platforms = ["linux/amd64", "linux/arm64"]
  context   = "madgraph"
  tags      = ["startown/madgraph:3.8.1-9"]
  attest    = ["type=provenance,mode=min", "type=sbom"]
}
