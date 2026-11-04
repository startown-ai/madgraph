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

COPY --from=startown/delphes:3.5.1@sha256:c1f4c5b1ca8fe92f29175261aa9ad982f1c54b867e5476aee4f0bfa2e7bdc122 /opt/delphes /opt/delphes
COPY --from=startown/emela:1.0.1@sha256:7c7012ce5bb3e30d4e9e6b662de0998956199e65461ff8a64750a186b92c56e0 /opt/emela /opt/emela
COPY --from=startown/fastjet:3.5.3@sha256:95f505458b9d80774cce886a0b7b9abee8becc82600bd0edf217d005b571676f /opt/fastjet /opt/fastjet
COPY --from=startown/hepmc:3.3.1@sha256:6de97b540451cf4683f54c3fee4e0bbdeabb631670593dd83e28b1d3e6d51c58 /opt/hepmc /opt/hepmc
COPY --from=startown/lhapdf:6.5.6@sha256:d4017b0b918a2107d10331933b13096664e9ce5f6b4b039bb43e96f71ef7272f /opt/lhapdf /opt/lhapdf
COPY --from=startown/pythia:8.317@sha256:0973d38d363224860a6bbf47f32cd9b5dbd5e391534af1160efc4e87b4f09201 /opt/pythia /opt/pythia
COPY --from=startown/root:6.40.04@sha256:738dd66d3128b56c89a2c6bb9ab085da890a6b842b26d0a28b412a302109d0a2 /opt/root /opt/root
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

WORKDIR /root
CMD ["bash"]
