FROM dhi.io/debian-base:trixie-dev@sha256:9e479c64f8d27f8fa368ef62804c2ddb9b3e43a68c13a200b2f668b4771b049e AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      make=4.4.1-2+deb13u0+dhi1 \
      rsync=3.5.0+ds1-0+deb13u1+dhi0 \
      zlib1g-dev=1:1.3.dfsg+really1.3.1-1+deb13u0+dhi4 && \
    rm -rf /var/lib/apt/lists/*

COPY --from=startown/hepmc:2.06.11@sha256:550a0217e96aa220f43e66a5c3a841dca42e7a6454e8fea8f5e12271a517672e /opt/hepmc /opt/hepmc
COPY --from=startown/lhapdf:6.5.6@sha256:e438106716cff60ed3495c2ebec16059b1e403db10cefd268c76582b43a7d161 /opt/lhapdf /opt/lhapdf
RUN echo /opt/hepmc/lib > /etc/ld.so.conf.d/hepmc.conf && \
    echo /opt/lhapdf/lib > /etc/ld.so.conf.d/lhapdf.conf && \
    ldconfig

ADD https://assets.startown.ai/packages/pythia/8.317.tar.gz /tmp/pythia.tgz

RUN mkdir /src && \
    tar -xzf /tmp/pythia.tgz -C /src --strip-components=1 && \
    cd /src && \
    ./configure --prefix=/opt/pythia \
      --with-hepmc2=/opt/hepmc \
      --with-lhapdf6=/opt/lhapdf \
      --with-gzip && \
    make -j"$(nproc)" && \
    make install

FROM build AS test

ADD https://lhapdfsets.web.cern.ch/current/NNPDF31_nnlo_as_0118_luxqed.tar.gz /tmp/pdfset.tar.gz

RUN tar -xzf /tmp/pdfset.tar.gz -C /opt/lhapdf/share/LHAPDF && \
    cd /src/examples && \
    make -j"$(nproc)" main101 main121 main131 main164 main201 && \
    ./main101 && \
    ./main121 | grep 'useGzip = 1' && \
    ./main131 && \
    grep -q '^HepMC::Version 2.06.11' main131.hepmc && \
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

COPY --from=build /opt/hepmc /opt/hepmc
COPY --from=build /opt/lhapdf /opt/lhapdf
COPY --from=build /opt/pythia /opt/pythia
ENV PATH=/opt/pythia/bin:/opt/lhapdf/bin:${PATH}
RUN echo /opt/hepmc/lib > /etc/ld.so.conf.d/hepmc.conf && \
    echo /opt/lhapdf/lib > /etc/ld.so.conf.d/lhapdf.conf && \
    echo /opt/pythia/lib > /etc/ld.so.conf.d/pythia.conf && \
    ldconfig && \
    echo /opt/lhapdf/lib/python3.13/dist-packages > /usr/lib/python3/dist-packages/lhapdf.pth

WORKDIR /root
CMD ["bash"]
