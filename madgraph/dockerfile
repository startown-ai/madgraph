FROM dhi.io/debian-base:trixie-dev@sha256:9e479c64f8d27f8fa368ef62804c2ddb9b3e43a68c13a200b2f668b4771b049e AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      make=4.4.1-2+deb13u0+dhi1 \
      python3=3.13.5-1 \
      zlib1g-dev=1:1.3.dfsg+really1.3.1-1+deb13u0+dhi4 && \
    rm -rf /var/lib/apt/lists/*

COPY --from=startown/pythia8:8317 /opt/hepmc2 /opt/hepmc2
COPY --from=startown/pythia8:8317 /opt/pythia8 /opt/pythia8

ADD https://assets.startown.ai/packages/madgraph/3.5.17.tar.gz /tmp/madgraph.tar.gz
ADD https://assets.startown.ai/packages/mg5amc-py8-interface/1.3.tar.gz /tmp/mg5amc-py8-interface.tar.gz

RUN mkdir -p /opt/madgraph/HEPTools/MG5aMC_PY8_interface && \
    tar -xzf /tmp/madgraph.tar.gz -C /opt/madgraph --strip-components=1 --exclude='._*' --warning=no-unknown-keyword && \
    tar -xzf /tmp/mg5amc-py8-interface.tar.gz -C /opt/madgraph/HEPTools/MG5aMC_PY8_interface && \
    cp -a /opt/pythia8 /tmp/pythia8 && \
    sed -i 's/^CXX_COMMON=.*/& -DHEPMC2HACK -DHEPMC2/' /tmp/pythia8/share/Pythia8/examples/Makefile.inc && \
    cd /opt/madgraph/HEPTools/MG5aMC_PY8_interface && \
    python3 compile.py /tmp/pythia8 /opt/madgraph && \
    rm -rf /tmp/pythia8

RUN sed -i 's/if os.path.getsize(path) > 1e9 or forceexternal:/if os.path.getsize(path) >= 0 or forceexternal:/' /opt/madgraph/madgraph/various/misc.py && \
    grep -q 'getsize(path) >= 0 or forceexternal' /opt/madgraph/madgraph/various/misc.py && \
    printf '%s\n' \
      'pythia8_path = /opt/pythia8' \
      'mg5amc_py8_interface_path = /opt/madgraph/HEPTools/MG5aMC_PY8_interface' \
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
      pigz=2.8-1+b1 \
      procps=2:4.0.4-9+deb13u0+dhi0 \
      python3=3.13.5-1 \
      python3-numpy=1:2.2.4+ds-1 \
      python3-six=1.17.0-1 \
      zlib1g-dev=1:1.3.dfsg+really1.3.1-1+deb13u0+dhi4 && \
    rm -rf /var/lib/apt/lists/*

COPY --from=startown/emela:1.0.1 /opt/emela /opt/emela
COPY --from=startown/fastjet:3.5.3 /opt/fastjet /opt/fastjet
COPY --from=startown/delphes:3.5.1 /opt/root /opt/root
COPY --from=startown/delphes:3.5.1 /opt/delphes /opt/delphes
COPY --from=startown/pythia8:8317 /opt/lhapdf /opt/lhapdf
COPY --from=build /opt/hepmc2 /opt/hepmc2
COPY --from=build /opt/pythia8 /opt/pythia8
COPY --from=build /opt/madgraph /opt/madgraph

ADD https://assets.startown.ai/packages/lhapdf-sets/NNPDF23_lo_as_0130_qed.tar.gz /tmp/pdfset.tar.gz

ENV PATH=/opt/madgraph/bin:/opt/delphes/bin:/opt/pythia8/bin:/opt/fastjet/bin:/opt/emela/bin:/opt/lhapdf/bin:/opt/root/bin:${PATH} \
    DELPHES_HOME=/opt/delphes \
    LD_LIBRARY_PATH=/opt/delphes/lib \
    LIBRARY_PATH=/opt/delphes/lib \
    ROOT_INCLUDE_PATH=/opt/delphes/include
RUN tar -xzf /tmp/pdfset.tar.gz -C /opt/lhapdf/share/LHAPDF && \
    rm /tmp/pdfset.tar.gz && \
    mkdir -p /usr/local/bin && \
    ln -s /usr/bin/pigz /usr/local/bin/gzip && \
    for p in hepmc2 lhapdf fastjet emela pythia8 root delphes; do \
      echo /opt/$p/lib > /etc/ld.so.conf.d/$p.conf; \
    done && \
    ldconfig && \
    echo /opt/lhapdf/lib/python3.13/dist-packages > /usr/lib/python3/dist-packages/lhapdf.pth && \
    echo /opt/root/lib > /usr/lib/python3/dist-packages/root.pth

WORKDIR /root
CMD ["bash"]

FROM runtime AS test

RUN printf '%s\n' \
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
    echo "HepMC weights: $(zcat tag_1_pythia8_events.hepmc.gz | grep -m1 '^N ' | cut -c1-120)" && \
    root -b -l -q -e 'TFile f("tag_1_delphes_events.root"); auto t = (TTree*)f.Get("Delphes"); std::cout << "Delphes events: " << t->GetEntries() << std::endl; gSystem->Exit(t->GetEntries() == 1000 ? 0 : 1);'

FROM runtime
