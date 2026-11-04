FROM dhi.io/debian-base:trixie-dev@sha256:af61a3296b0c92f16969f11ce8c2a93722e84c9e5c1bb42ed6afec796be71b04 AS build

ADD https://assets.startown.ai/packages/madgraph/3.8.1.tar.gz /tmp/madgraph.tar.gz

RUN mkdir -p /opt/madgraph && \
    tar -xzf /tmp/madgraph.tar.gz -C /opt/madgraph --strip-components=1 --exclude='._*' --warning=no-unknown-keyword && \
    sed -i 's/DelphesHepMC2/DelphesHepMC3/g' \
      /opt/madgraph/madgraph/interface/madevent_interface.py \
      /opt/madgraph/Template/LO/bin/internal/run_delphes3 && \
    grep -q DelphesHepMC3 /opt/madgraph/madgraph/interface/madevent_interface.py && \
    grep -q DelphesHepMC3 /opt/madgraph/Template/LO/bin/internal/run_delphes3 && \
    sed -i 's/^HEPMCoutput:file *= hepmc\.gz$/HEPMCoutput:file         = hepmcremove/' \
      /opt/madgraph/Template/LO/Cards/pythia8_card_default.dat && \
    sed -i "s/add_param(\"HEPMCoutput:file\", 'hepmc\.gz')/add_param(\"HEPMCoutput:file\", 'hepmcremove')/" \
      /opt/madgraph/madgraph/various/banner.py && \
    grep -q '^HEPMCoutput:file *= hepmcremove$' /opt/madgraph/Template/LO/Cards/pythia8_card_default.dat && \
    grep -q "add_param(\"HEPMCoutput:file\", 'hepmcremove')" /opt/madgraph/madgraph/various/banner.py && \
    printf '%s\n' \
      'pythia8_path = /opt/pythia' \
      'delphes_path = /opt/delphes/bin' \
      'lhapdf_py3 = /opt/lhapdf/bin/lhapdf-config' \
      'fastjet = /opt/fastjet/bin/fastjet-config' \
      'eMELA = /opt/emela/bin/eMELA-config' \
      'nb_core_delphes = 1' \
      'auto_update = 0' \
      'automatic_html_opening = False' \
      >> /opt/madgraph/input/mg5_configuration.txt

FROM dhi.io/debian-base:trixie-dev@sha256:af61a3296b0c92f16969f11ce8c2a93722e84c9e5c1bb42ed6afec796be71b04

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      bc=1.07.1-4+deb13u0+dhi0 \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      gfortran=4:14.2.0-1+deb13u0+dhi0 \
      libfreetype6=2.13.3+dfsg-1+deb13u1+dhi1 \
      libpython3.13=3.13.5-2+deb13u5 \
      make=4.4.1-2+deb13u0+dhi1 \
      perl=5.40.1-6+deb13u1+dhi3 \
      'pigz=2.8-1*' \
      procps=2:4.0.4-9+deb13u0+dhi0 \
      python3=3.13.5-1 \
      python3-numpy=1:2.2.4+ds-1 \
      zlib1g-dev=1:1.3.dfsg+really1.3.1-1+deb13u0+dhi4 && \
    rm -rf /var/lib/apt/lists/*

COPY --from=startown/delphes:3.5.1@sha256:b2a25ae79be4b46e72dae8960c883333bd66c5d1200bc0e1574e5449da8f963b /opt/delphes /opt/delphes
COPY --from=startown/emela:1.0.1@sha256:80d0341843003aebebf490b132f9040db611118fd8d6af8df87222c5803c8d34 /opt/emela /opt/emela
COPY --from=startown/fastjet:3.5.3@sha256:d63e9b1768aa24d6fbe06f53c26da954873f85d23acff76b62a20861f2cefb48 /opt/fastjet /opt/fastjet
COPY --from=startown/hepmc:3.3.1@sha256:462f162d9071cd4d2f1be0f58f2ab287bd8c5389f319027fa2fe48276dc35e33 /opt/hepmc /opt/hepmc
COPY --from=startown/lhapdf:6.5.6@sha256:fa5f95c1bc478f819c3ed727fca9256981ab17cb0c963caf8e0a5ece975aae9f /opt/lhapdf /opt/lhapdf
COPY --from=startown/pythia:8.317@sha256:20076dc904f245b3e8bc001811af2eaaaf09763d062d5e7251b9db83214608ca /opt/pythia /opt/pythia
COPY --from=startown/root:6.40.04@sha256:2f5dfd51547487aaa48d19dc9a97cd4b4dfba08c4af938559f4216eacf57b340 /opt/root /opt/root
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
    echo /opt/lhapdf/lib/python3.13/dist-packages > /usr/lib/python3/dist-packages/lhapdf.pth

WORKDIR /data
CMD ["bash"]
