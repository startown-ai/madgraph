FROM dhi.io/debian-base:trixie-dev@sha256:94571f3dc07fbc7166f6f1cbf1fe068a7647198ee2554e6b90d59a68cc8b053c AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      patch=2.8-2+deb13u0+dhi3

ADD --checksum=sha256:281b9055af99f45217dc99378c520215f3129e65a9ab2fde6636e274aa6cd692 https://assets.startown.ai/packages/madgraph/3.8.1.tar.gz /tmp/madgraph.tar.gz
COPY *.patch /tmp/

RUN mkdir /opt/madgraph && \
    tar -xzf /tmp/madgraph.tar.gz -C /opt/madgraph --strip-components=1 --exclude='._*' --warning=no-unknown-keyword && \
    patch -d /opt/madgraph -p1 < /tmp/quiet-warnings.patch && \
    patch -d /opt/madgraph -p1 < /tmp/seeds.patch && \
    patch -d /opt/madgraph -p1 < /tmp/cores.patch && \
    patch -d /opt/madgraph -p1 < /tmp/delphes-stream.patch && \
    patch -d /opt/madgraph -p1 < /tmp/fast-lhe-split.patch && \
    patch -d /opt/madgraph -p1 < /tmp/hadd-no-j.patch && \
    patch -d /opt/madgraph -p1 < /tmp/delphes-inprocess.patch && \
    patch -d /opt/madgraph -p1 < /tmp/delphes-hepmc3.patch && \
    patch -d /opt/madgraph -p1 < /tmp/hepmc-autoremove.patch && \
    printf '%s\n' \
      'pythia8_path = /opt/pythia' \
      'delphes_path = /opt/delphes/bin' \
      'lhapdf_py3 = /opt/lhapdf/bin/lhapdf-config' \
      'fastjet = /opt/fastjet/bin/fastjet-config' \
      'eMELA = /opt/emela/bin/eMELA-config' \
      'auto_update = 0' \
      'automatic_html_opening = False' \
      >> /opt/madgraph/input/mg5_configuration.txt
RUN rm /opt/madgraph/madgraph/interface/madevent_interface.py.orig

FROM dhi.io/debian-base:trixie-dev@sha256:94571f3dc07fbc7166f6f1cbf1fe068a7647198ee2554e6b90d59a68cc8b053c

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      bc=1.07.1-4+deb13u0+dhi0 \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      gfortran=4:14.2.0-1+deb13u0+dhi0 \
      make=4.4.1-2+deb13u0+dhi1 \
      ninja-build=1.12.1-1* \
      perl=5.40.1-6+deb13u1+dhi3 \
      pigz=2.8-1* \
      procps=2:4.0.4-9+deb13u0+dhi0 \
      python-3.13=3.13.18-7 \
      python-3.13-dev=3.13.18-7 \
      python3-pip=26.3-0 \
      zlib1g-dev=1:1.3.dfsg+really1.3.1-1+deb13u1+dhi0 && \
    rm -rf /var/lib/apt/lists/*
RUN printf '%s\n' \
      'meson==1.7.0 --hash=sha256:ae3f12953045f3c7c60e27f2af1ad862f14dee125b4ed9bcb8a842a5080dbf85' \
      'numpy==2.2.4 --hash=sha256:6f527d8fdb0286fd2fd97a2a96c6be17ba4232da346931d967a0630050dfd298 --hash=sha256:bce43e386c16898b91e162e5baaad90c4b06f9dcbe36282490032cec98dc8ae7' | \
    pip install --no-cache-dir --require-hashes -r /dev/stdin

COPY --from=docker.io/startown/delphes:3.5.1-1@sha256:ef9473facad498c84128643287e266a09bc12138f70d52036e349b8dd95d0ee7 /opt/delphes /opt/delphes
COPY --from=docker.io/startown/emela:1.0.1-1@sha256:99f30390a06b53e36613fedb3984a4e820e487e0530a97bc4a9b27150b3e46d1 /opt/emela /opt/emela
COPY --from=docker.io/startown/fastjet:3.5.3-1@sha256:70f538596ab6919760e1fbff9a0513b1b1f218d5550f96dcec8a62c7d7591b23 /opt/fastjet /opt/fastjet
COPY --from=docker.io/startown/hepmc:3.3.1-1@sha256:a7196e9d9031051d7d1dfd177fb4b1aad8ba8a7864d43f4bec186c3c682d0f3e /opt/hepmc /opt/hepmc
COPY --from=docker.io/startown/lhapdf:6.5.6-2@sha256:e428a4334e637e794682d3f7511ceff1fcbb269a002e3237eaa7981000c99e2c /opt/lhapdf /opt/lhapdf
COPY --from=docker.io/startown/pythia:8.317-2@sha256:d2066d95f0eab29469b64bcd45c4cfcb8f6586ff88feafcfd7bb5f98449b5ef9 /opt/pythia /opt/pythia
COPY --from=docker.io/startown/root:6.36.14-1@sha256:95e00b7d3d196d4f3de275022d5718844f86bb03413741e25dee76e85c5ce746 /opt/root /opt/root
COPY --from=build /opt/madgraph /opt/madgraph
ENV PATH=/opt/madgraph/bin:/opt/delphes/bin:/opt/emela/bin:/opt/fastjet/bin:/opt/hepmc/bin:/opt/lhapdf/bin:/opt/pythia/bin:/opt/root/bin:${PATH} \
    LD_LIBRARY_PATH=/opt/delphes/lib \
    PIGZ=-1 \
    ROOT_INCLUDE_PATH=/opt/delphes/include
RUN printf '/opt/%s/lib\n' delphes emela fastjet hepmc lhapdf pythia root > /etc/ld.so.conf.d/opt.conf && ldconfig

CMD ["bash"]
