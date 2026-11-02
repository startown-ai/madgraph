FROM dhi.io/debian-base:trixie-dev@sha256:9e479c64f8d27f8fa368ef62804c2ddb9b3e43a68c13a200b2f668b4771b049e AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      make=4.4.1-2+deb13u0+dhi1 \
      rsync=3.5.0+ds1-0+deb13u1+dhi0 \
      zlib1g-dev=1:1.3.dfsg+really1.3.1-1+deb13u0+dhi4 && \
    rm -rf /var/lib/apt/lists/*

COPY --from=startown/hepmc:3.3.1@sha256:4ea2c370d33d6b44c8375ea06832a1fe051cc32e8dc4995433b13a3631f4d8da /opt/hepmc3 /opt/hepmc3
COPY --from=startown/lhapdf:6.5.6@sha256:e438106716cff60ed3495c2ebec16059b1e403db10cefd268c76582b43a7d161 /opt/lhapdf /opt/lhapdf
RUN echo /opt/hepmc3/lib > /etc/ld.so.conf.d/hepmc3.conf && \
    echo /opt/lhapdf/lib > /etc/ld.so.conf.d/lhapdf.conf && \
    ldconfig

ADD https://assets.startown.ai/packages/pythia/8.317.tar.gz /tmp/pythia8.tgz

RUN mkdir /src && \
    tar -xzf /tmp/pythia8.tgz -C /src --strip-components=1 && \
    cd /src && \
    ./configure --prefix=/opt/pythia8 \
      --with-hepmc3=/opt/hepmc3 \
      --with-lhapdf6=/opt/lhapdf \
      --with-gzip && \
    make -j"$(nproc)" && \
    make install && \
    make -C /opt/pythia8/share/Pythia8/examples main164

FROM build AS test

ADD https://lhapdfsets.web.cern.ch/current/NNPDF31_nnlo_as_0118_luxqed.tar.gz /tmp/pdfset.tar.gz

RUN tar -xzf /tmp/pdfset.tar.gz -C /opt/lhapdf/share/LHAPDF && \
    cd /src/examples && \
    make -j"$(nproc)" main101 main121 main131 main164 main201 && \
    ./main101 && \
    ./main121 | grep 'useGzip = 1' && \
    ./main131 && \
    grep -q '^HepMC::Version 3.03.01' main131.hepmc && \
    ldd /opt/pythia8/share/Pythia8/examples/main164 | grep -q libHepMC3 && \
    ./main164 -c main164mlm.cmnd && \
    ./main201

FROM dhi.io/debian-base:trixie-dev@sha256:9e479c64f8d27f8fa368ef62804c2ddb9b3e43a68c13a200b2f668b4771b049e

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      libpython3.13=3.13.5-2+deb13u5 \
      make=4.4.1-2+deb13u0+dhi1 \
      python3=3.13.5-1 \
      zlib1g-dev=1:1.3.dfsg+really1.3.1-1+deb13u0+dhi4 && \
    rm -rf /var/lib/apt/lists/*

COPY --from=build /opt/hepmc3 /opt/hepmc3
COPY --from=build /opt/lhapdf /opt/lhapdf
COPY --from=build /opt/pythia8 /opt/pythia8
ENV PATH=/opt/pythia8/bin:/opt/hepmc3/bin:/opt/lhapdf/bin:${PATH}
RUN echo /opt/hepmc3/lib > /etc/ld.so.conf.d/hepmc3.conf && \
    echo /opt/lhapdf/lib > /etc/ld.so.conf.d/lhapdf.conf && \
    echo /opt/pythia8/lib > /etc/ld.so.conf.d/pythia8.conf && \
    ldconfig && \
    echo /opt/lhapdf/lib/python3.13/dist-packages > /usr/lib/python3/dist-packages/lhapdf.pth

WORKDIR /root
CMD ["bash"]
