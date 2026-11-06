FROM dhi.io/debian-base:trixie-dev@sha256:61025cea33d5910165ced8b33edd3f6b6bfe161e129174887936a1b9a1442b20 AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      patch=2.8-2+deb13u0+dhi3 && \
    rm -rf /var/lib/apt/lists/*

ADD https://assets.startown.ai/packages/madgraph/3.8.1.tar.gz /tmp/madgraph.tar.gz
COPY quiet-warnings.patch /tmp/quiet-warnings.patch

RUN mkdir -p /opt/madgraph && \
    tar -xzf /tmp/madgraph.tar.gz -C /opt/madgraph --strip-components=1 --exclude='._*' --warning=no-unknown-keyword && \
    patch -d /opt/madgraph -p1 < /tmp/quiet-warnings.patch && \
    sed -i 's/DelphesHepMC2/DelphesHepMC3/g' \
      /opt/madgraph/madgraph/interface/madevent_interface.py \
      /opt/madgraph/Template/LO/bin/internal/run_delphes3 && \
    printf '%s\n' \
      'pythia8_path = /opt/pythia' \
      'delphes_path = /opt/delphes/bin' \
      'lhapdf_py3 = /opt/lhapdf/bin/lhapdf-config' \
      'fastjet = /opt/fastjet/bin/fastjet-config' \
      'eMELA = /opt/emela/bin/eMELA-config' \
      'auto_update = 0' \
      'automatic_html_opening = False' \
      >> /opt/madgraph/input/mg5_configuration.txt

FROM dhi.io/debian-base:trixie-dev@sha256:61025cea33d5910165ced8b33edd3f6b6bfe161e129174887936a1b9a1442b20

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      bc=1.07.1-4+deb13u0+dhi0 \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      gfortran=4:14.2.0-1+deb13u0+dhi0 \
      libfreetype6=2.13.3+dfsg-1+deb13u1+dhi1 \
      libftgl2=2.4.0-5* \
      libgif7=5.2.2-1+deb13u1 \
      libglew2.2=2.2.0-4* \
      libglu1-mesa=9.0.2-1.1* \
      libjpeg62-turbo=1:2.1.5-4+deb13u0+dhi0 \
      liblz4-1=1.10.0-4+deb13u0+dhi3 \
      libpcre2-8-0=10.46-1~deb13u3+dhi0 \
      libpython3.13=3.13.5-2+deb13u5 \
      libtbb12=2022.1.0-1+deb13u1 \
      libvdt-dev=0.4.4-4 \
      libx11-6=2:1.8.12-1+dhi0 \
      libxext6=2:1.3.4-1+dhi0 \
      libxft2=2.3.6-1+dhi0 \
      libxpm4=1:3.5.17-1+deb13u1+dhi1 \
      libxxhash0=0.8.3-2+deb13u0+dhi1 \
      make=4.4.1-2+deb13u0+dhi1 \
      meson=1.7.0-1 \
      ninja-build=1.12.1-1* \
      pigz=2.8-1* \
      procps=2:4.0.4-9+deb13u0+dhi0 \
      python3=3.13.5-1 \
      python3-dev=3.13.5-1 \
      python3-numpy=1:2.2.4+ds-1 \
      zlib1g-dev=1:1.3.dfsg+really1.3.1-1+deb13u0+dhi4 && \
    rm -rf /var/lib/apt/lists/*

COPY --from=delphes /opt/delphes /opt/delphes
COPY --from=emela /opt/emela /opt/emela
COPY --from=fastjet /opt/fastjet /opt/fastjet
COPY --from=hepmc /opt/hepmc /opt/hepmc
COPY --from=lhapdf /opt/lhapdf /opt/lhapdf
COPY --from=pythia /opt/pythia /opt/pythia
COPY --from=root /opt/root /opt/root
COPY --from=build /opt/madgraph /opt/madgraph

ADD https://assets.startown.ai/packages/lhapdf-sets/NNPDF23_lo_as_0130_qed.tar.gz /tmp/pdfset.tar.gz

ENV PATH=/opt/madgraph/bin:/opt/delphes/bin:/opt/pythia/bin:/opt/hepmc/bin:/opt/fastjet/bin:/opt/emela/bin:/opt/lhapdf/bin:/opt/root/bin:${PATH} \
    DELPHES_HOME=/opt/delphes \
    LD_LIBRARY_PATH=/opt/delphes/lib \
    LIBRARY_PATH=/opt/delphes/lib \
    PIGZ=-1 \
    ROOT_INCLUDE_PATH=/opt/delphes/include
RUN tar -xzf /tmp/pdfset.tar.gz -C /opt/lhapdf/share/LHAPDF && \
    rm /tmp/pdfset.tar.gz && \
    for p in hepmc lhapdf fastjet emela pythia root delphes; do \
      echo /opt/$p/lib > /etc/ld.so.conf.d/$p.conf; \
    done && \
    ldconfig && \
    echo /opt/lhapdf/lib/python3.13/dist-packages > /usr/lib/python3/dist-packages/lhapdf.pth && \
    echo /opt/root/lib > /usr/lib/python3/dist-packages/root.pth

WORKDIR /root
CMD ["bash"]
