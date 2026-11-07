FROM dhi.io/debian-base:trixie-dev@sha256:61025cea33d5910165ced8b33edd3f6b6bfe161e129174887936a1b9a1442b20 AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      patch=2.8-2+deb13u0+dhi3 && \
    rm -rf /var/lib/apt/lists/*

ADD https://assets.startown.ai/packages/madgraph/3.8.1.tar.gz /tmp/madgraph.tar.gz
COPY quiet-warnings.patch \
     seeds.patch \
     cores.patch \
     delphes-stream.patch \
     fast-lhe-split.patch \
     hadd-no-j.patch \
     delphes-inprocess.patch \
     /tmp/

RUN mkdir -p /opt/madgraph && \
    tar -xzf /tmp/madgraph.tar.gz -C /opt/madgraph --strip-components=1 --exclude='._*' --warning=no-unknown-keyword && \
    patch -d /opt/madgraph -p1 < /tmp/quiet-warnings.patch && \
    patch -d /opt/madgraph -p1 < /tmp/seeds.patch && \
    patch -d /opt/madgraph -p1 < /tmp/cores.patch && \
    patch -d /opt/madgraph -p1 < /tmp/delphes-stream.patch && \
    patch -d /opt/madgraph -p1 < /tmp/fast-lhe-split.patch && \
    patch -d /opt/madgraph -p1 < /tmp/hadd-no-j.patch && \
    patch -d /opt/madgraph -p1 < /tmp/delphes-inprocess.patch && \
    sed -i 's/DelphesHepMC2/DelphesHepMC3/g' \
      /opt/madgraph/madgraph/interface/madevent_interface.py \
      /opt/madgraph/Template/LO/bin/internal/run_delphes3 && \
    sed -i 's/^HEPMCoutput:file *= hepmc\.gz$/HEPMCoutput:file         = autoremove/' \
      /opt/madgraph/Template/LO/Cards/pythia8_card_default.dat && \
    sed -i "s/add_param(\"HEPMCoutput:file\", 'hepmc\.gz')/add_param(\"HEPMCoutput:file\", 'autoremove')/" \
      /opt/madgraph/madgraph/various/banner.py && \
    printf '%s\n' \
      'pythia8_path = /opt/pythia' \
      'delphes_path = /opt/delphes/bin' \
      'lhapdf_py3 = /opt/lhapdf/bin/lhapdf-config' \
      'fastjet = /opt/fastjet/bin/fastjet-config' \
      'eMELA = /opt/emela/bin/eMELA-config' \
      'auto_update = 0' \
      'automatic_html_opening = False' \
      >> /opt/madgraph/input/mg5_configuration.txt

FROM dhi.io/debian-base:trixie-dev@sha256:61025cea33d5910165ced8b33edd3f6b6bfe161e129174887936a1b9a1442b20 AS runtime

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      bc=1.07.1-4+deb13u0+dhi0 \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      gfortran=4:14.2.0-1+deb13u0+dhi0 \
      libexpat1=2.9.0-1~deb13u1+dhi1 \
      libffi8=3.4.8-2+deb13u0+dhi2 \
      libfreetype6=2.13.3+dfsg-1+deb13u1+dhi1 \
      libncursesw6=6.5+20250216-2+deb13u0+dhi5 \
      libreadline8t64=8.2-6+deb13u0+dhi2 \
      make=4.4.1-2+deb13u0+dhi1 \
      ninja-build=1.12.1-1* \
      perl=5.40.1-6+deb13u1+dhi3 \
      pigz=2.8-1* \
      procps=2:4.0.4-9+deb13u0+dhi0 \
      zlib1g-dev=1:1.3.dfsg+really1.3.1-1+deb13u0+dhi4 && \
    rm -rf /var/lib/apt/lists/*

COPY --from=binutils /opt/binutils /opt/binutils
COPY --from=delphes /opt/delphes /opt/delphes
COPY --from=emela /opt/emela /opt/emela
COPY --from=fastjet /opt/fastjet /opt/fastjet
COPY --from=hepmc /opt/hepmc /opt/hepmc
COPY --from=lhapdf /opt/lhapdf /opt/lhapdf
COPY --from=pythia /opt/pythia /opt/pythia
COPY --from=python /opt/python /opt/python
COPY --from=root /opt/root /opt/root
COPY --from=build /opt/madgraph /opt/madgraph

COPY requirements.txt /tmp/requirements.txt
RUN pip=$(ls /opt/python/lib/python3.13/ensurepip/_bundled/pip-*.whl) && \
    /opt/python/bin/python3 $pip/pip install \
      --no-cache-dir \
      --no-deps \
      --only-binary=:all: \
      --require-hashes \
      --root-user-action=ignore \
      --disable-pip-version-check \
      -r /tmp/requirements.txt && \
    rm /tmp/requirements.txt

ADD https://assets.startown.ai/packages/lhapdf-sets/NNPDF23_lo_as_0130_qed.tar.gz /tmp/pdfset.tar.gz

ENV PATH=/opt/python/bin:/opt/madgraph/bin:/opt/delphes/bin:/opt/pythia/bin:/opt/hepmc/bin:/opt/fastjet/bin:/opt/emela/bin:/opt/lhapdf/bin:/opt/root/bin:/opt/binutils/bin:${PATH} \
    DELPHES_HOME=/opt/delphes \
    LD_LIBRARY_PATH=/opt/delphes/lib \
    LIBRARY_PATH=/opt/delphes/lib \
    PIGZ=-1 \
    ROOT_INCLUDE_PATH=/opt/delphes/include
RUN tar -xzf /tmp/pdfset.tar.gz -C /opt/lhapdf/share/LHAPDF && \
    rm /tmp/pdfset.tar.gz && \
    for p in python hepmc lhapdf fastjet emela pythia root delphes; do \
      echo /opt/$p/lib > /etc/ld.so.conf.d/$p.conf; \
    done && \
    ldconfig && \
    echo /opt/lhapdf/lib/python3.13/dist-packages > /opt/python/lib/python3.13/site-packages/lhapdf.pth

CMD ["bash"]

FROM runtime AS shower

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      patch=2.8-2+deb13u0+dhi3 && \
    rm -rf /var/lib/apt/lists/*

COPY main164delphes.patch DelphesPythia8Event.h /tmp/shower/

RUN cd /tmp/shower && \
    cp /opt/pythia/share/Pythia8/examples/main164.cc . && \
    patch -p1 -o main164delphes.cc < main164delphes.patch && \
    g++ main164delphes.cc -o main164delphes -O2 -std=c++17 -DGZIP -DHEPMC3 \
      -I. -I/opt/pythia/include -I/opt/hepmc/include -I/opt/delphes/include $(root-config --cflags) \
      -L/opt/pythia/lib -Wl,-rpath,/opt/pythia/lib -lpythia8 \
      -L/opt/hepmc/lib -Wl,-rpath,/opt/hepmc/lib -lHepMC3 \
      -L/opt/delphes/lib -Wl,-rpath,/opt/delphes/lib -lDelphes \
      $(root-config --libs) -lEG -ldl -lz

FROM runtime

COPY --from=shower /tmp/shower/main164delphes /opt/pythia/share/Pythia8/examples/main164delphes

RUN pkgs="binutils binutils-common binutils-$(gcc -dumpmachine | tr _ -) libbinutils libctf0 libctf-nobfd0 libgprofng0 libsframe1 libjansson4" && \
    dpkg --purge --force-depends $pkgs && \
    for p in $pkgs; do rm -f /var/lib/dpkg/status.d/$p; done

RUN pkgs="libfreetype6 libpng16-16t64" && \
    dpkg --purge $pkgs && \
    for p in $pkgs; do rm -f /var/lib/dpkg/status.d/$p; done && \
    ldconfig

RUN pkgs="apt libapt-pkg7.0 gpgv libgcrypt20 libgpg-error0 libseccomp2 debian-archive-keyring" && \
    dpkg --purge $pkgs && \
    for p in $pkgs; do rm -f /var/lib/dpkg/status.d/$p; done && \
    rm -rf /var/lib/apt /var/cache/apt
