FROM dhi.io/debian-base:trixie-dev@sha256:61025cea33d5910165ced8b33edd3f6b6bfe161e129174887936a1b9a1442b20 AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      make=4.4.1-2+deb13u0+dhi1 \
      python3=3.13.5-1 \
      zlib1g-dev=1:1.3.dfsg+really1.3.1-1+deb13u0+dhi4 && \
    rm -rf /var/lib/apt/lists/*

COPY --from=hepmc /opt/hepmc /opt/hepmc
COPY --from=pythia /opt/pythia /opt/pythia

ADD https://assets.startown.ai/packages/madgraph/3.5.17.tar.gz /tmp/madgraph.tar.gz
ADD https://assets.startown.ai/packages/mg5amc-py8-interface/1.3.tar.gz /tmp/mg5amc-py8-interface.tar.gz

RUN mkdir -p /opt/madgraph/HEPTools/MG5aMC_PY8_interface && \
    tar -xzf /tmp/madgraph.tar.gz -C /opt/madgraph --strip-components=1 --exclude='._*' --warning=no-unknown-keyword && \
    tar -xzf /tmp/mg5amc-py8-interface.tar.gz -C /opt/madgraph/HEPTools/MG5aMC_PY8_interface && \
    cd /opt/madgraph/HEPTools/MG5aMC_PY8_interface && \
    python3 compile.py /opt/pythia /opt/madgraph && \
    printf '%s\n' \
      'pythia8_path = /opt/pythia' \
      'mg5amc_py8_interface_path = /opt/madgraph/HEPTools/MG5aMC_PY8_interface' \
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
      procps=2:4.0.4-9+deb13u0+dhi0 \
      python3=3.13.5-1 \
      python3-numpy=1:2.2.4+ds-1 \
      python3-six=1.17.0-1 \
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

ENV PATH=/opt/madgraph/bin:/opt/delphes/bin:/opt/pythia/bin:/opt/fastjet/bin:/opt/emela/bin:/opt/lhapdf/bin:/opt/root/bin:${PATH} \
    DELPHES_HOME=/opt/delphes \
    LD_LIBRARY_PATH=/opt/delphes/lib \
    LIBRARY_PATH=/opt/delphes/lib \
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
