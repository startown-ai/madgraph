FROM dhi.io/debian-base:trixie-dev@sha256:9e479c64f8d27f8fa368ef62804c2ddb9b3e43a68c13a200b2f668b4771b049e AS build

ADD https://assets.startown.ai/packages/madgraph/3.8.1.tar.gz /tmp/madgraph.tar.gz

RUN mkdir -p /opt/madgraph && \
    tar -xzf /tmp/madgraph.tar.gz -C /opt/madgraph --strip-components=1 --exclude='._*' --warning=no-unknown-keyword && \
    sed -i 's/DelphesHepMC2/DelphesHepMC3/g' \
      /opt/madgraph/madgraph/interface/madevent_interface.py \
      /opt/madgraph/Template/LO/bin/internal/run_delphes3 && \
    grep -q DelphesHepMC3 /opt/madgraph/madgraph/interface/madevent_interface.py && \
    grep -q DelphesHepMC3 /opt/madgraph/Template/LO/bin/internal/run_delphes3 && \
    printf '%s\n' \
      'pythia8_path = /opt/pythia8' \
      'delphes_path = /opt/delphes/bin' \
      'lhapdf_py3 = /opt/lhapdf/bin/lhapdf-config' \
      'fastjet = /opt/fastjet/bin/fastjet-config' \
      'eMELA = /opt/emela/bin/eMELA-config' \
      'auto_update = 0' \
      'automatic_html_opening = False' \
      >> /opt/madgraph/input/mg5_configuration.txt

FROM dhi.io/debian-base:trixie-dev@sha256:9e479c64f8d27f8fa368ef62804c2ddb9b3e43a68c13a200b2f668b4771b049e AS runtime

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      bc=1.07.1-4+deb13u0+dhi0 \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      gfortran=4:14.2.0-1+deb13u0+dhi0 \
      libfftw3-double3=3.3.10-2+dhi0 \
      libfreetype6=2.13.3+dfsg-1+deb13u1+dhi1 \
      libpython3.13=3.13.5-2+deb13u5 \
      libtbb12=2022.1.0-1+deb13u1 \
      libvdt-dev=0.4.4-4 \
      libxml2=2.12.7+dfsg+really2.9.14-2.1+deb13u3+dhi3 \
      make=4.4.1-2+deb13u0+dhi1 \
      perl=5.40.1-6+deb13u1+dhi3 \
      'pigz=2.8-1*' \
      procps=2:4.0.4-9+deb13u0+dhi0 \
      python3=3.13.5-1 \
      python3-numpy=1:2.2.4+ds-1 \
      zlib1g-dev=1:1.3.dfsg+really1.3.1-1+deb13u0+dhi4 && \
    rm -rf /var/lib/apt/lists/*

COPY --from=startown/emela:1.0.1@sha256:05900d7a5a03fd4ff39467e73a2af8f3fb2c7fc39a0e98be4fa4a2e636433069 /opt/emela /opt/emela
COPY --from=startown/fastjet:3.5.3@sha256:2a0a83d7bdc02aa394bbac62a2e13dd32fa6d0edf3d3542a80a206a28da45d92 /opt/fastjet /opt/fastjet
COPY --from=startown/delphes:3.5.1@sha256:5ef60db63a322c6ffb38ffdf87e613c5531ea276232d5524711b0635ffb66259 /opt/root /opt/root
COPY --from=startown/delphes:3.5.1@sha256:5ef60db63a322c6ffb38ffdf87e613c5531ea276232d5524711b0635ffb66259 /opt/delphes /opt/delphes
COPY --from=startown/pythia:8.317-hepmc3.3.1@sha256:735147963ac14464059a38ac24507c2f5f7640e1901e9f269bfa4fc9aacf45ec /opt/hepmc3 /opt/hepmc3
COPY --from=startown/pythia:8.317-hepmc3.3.1@sha256:735147963ac14464059a38ac24507c2f5f7640e1901e9f269bfa4fc9aacf45ec /opt/lhapdf /opt/lhapdf
COPY --from=startown/pythia:8.317-hepmc3.3.1@sha256:735147963ac14464059a38ac24507c2f5f7640e1901e9f269bfa4fc9aacf45ec /opt/pythia8 /opt/pythia8
COPY --from=build /opt/madgraph /opt/madgraph

ADD https://assets.startown.ai/packages/lhapdf-sets/NNPDF23_lo_as_0130_qed.tar.gz /tmp/pdfset.tar.gz

ENV PATH=/opt/madgraph/bin:/opt/delphes/bin:/opt/pythia8/bin:/opt/hepmc3/bin:/opt/fastjet/bin:/opt/emela/bin:/opt/lhapdf/bin:/opt/root/bin:${PATH} \
    DELPHES_HOME=/opt/delphes \
    LD_LIBRARY_PATH=/opt/delphes/lib \
    LIBRARY_PATH=/opt/delphes/lib \
    ROOT_INCLUDE_PATH=/opt/delphes/include
RUN tar -xzf /tmp/pdfset.tar.gz -C /opt/lhapdf/share/LHAPDF && \
    rm /tmp/pdfset.tar.gz && \
    mkdir -p /usr/local/bin && \
    ln -s /usr/bin/pigz /usr/local/bin/gzip && \
    for p in hepmc3 lhapdf fastjet emela pythia8 root delphes; do \
      echo /opt/$p/lib > /etc/ld.so.conf.d/$p.conf; \
    done && \
    ldconfig && \
    echo /opt/lhapdf/lib/python3.13/dist-packages > /usr/lib/python3/dist-packages/lhapdf.pth && \
    echo /opt/root/lib > /usr/lib/python3/dist-packages/root.pth

WORKDIR /root
CMD ["bash"]

FROM runtime AS test

RUN printf '%s\n' \
      'set nb_core_delphes 2' \
      'generate p p > t t~' \
      'output /tmp/ttbar' \
      'launch /tmp/ttbar' \
      'shower=Pythia8' \
      'detector=Delphes' \
      'set nevents 1000' \
      'set iseed 42' \
      > /tmp/ttbar.mg5 && \
    mg5_aMC /tmp/ttbar.mg5 && \
    cd /tmp/ttbar/Events/run_01 && \
    ls -l && \
    grep 'Integrated weight (pb)' run_01_tag_1_banner.txt && \
    echo "LHE events: $(zcat unweighted_events.lhe.gz | grep -c '<event')" && \
    zcat tag_1_pythia8_events.hepmc.gz | head -1 | grep -qx 'HepMC::Version 3.03.01' && \
    test "$(zcat tag_1_pythia8_events.hepmc.gz | grep -m1 '^W ' | tr '|' '\n' | wc -l)" -eq 146 && \
    root -b -l -q -e 'TFile f("tag_1_delphes_events.root"); auto t = (TTree*)f.Get("Delphes"); std::cout << "Delphes events: " << t->GetEntries() << std::endl; gSystem->Exit(t->GetEntries() == 1000 ? 0 : 1);'

FROM runtime
