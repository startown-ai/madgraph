FROM dhi.io/debian-base:trixie-dev@sha256:af61a3296b0c92f16969f11ce8c2a93722e84c9e5c1bb42ed6afec796be71b04 AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      make=4.4.1-2+deb13u0+dhi1 \
      rsync=3.5.0+ds1-0+deb13u1+dhi0 \
      zlib1g-dev=1:1.3.dfsg+really1.3.1-1+deb13u0+dhi4 && \
    rm -rf /var/lib/apt/lists/*

COPY --from=startown/hepmc:3.3.1@sha256:6de97b540451cf4683f54c3fee4e0bbdeabb631670593dd83e28b1d3e6d51c58 /opt/hepmc /opt/hepmc
COPY --from=startown/lhapdf:6.5.6@sha256:d4017b0b918a2107d10331933b13096664e9ce5f6b4b039bb43e96f71ef7272f /opt/lhapdf /opt/lhapdf
RUN echo /opt/hepmc/lib > /etc/ld.so.conf.d/hepmc.conf && \
    echo /opt/lhapdf/lib > /etc/ld.so.conf.d/lhapdf.conf && \
    ldconfig

ADD https://assets.startown.ai/packages/pythia/8.317.tar.gz /tmp/pythia.tgz

RUN mkdir /src && \
    tar -xzf /tmp/pythia.tgz -C /src --strip-components=1 && \
    cd /src && \
    ./configure --prefix=/opt/pythia \
      --with-hepmc3=/opt/hepmc \
      --with-lhapdf6=/opt/lhapdf \
      --with-gzip && \
    make -j"$(nproc)" && \
    make install && \
    make -C /opt/pythia/share/Pythia8/examples main164

FROM build AS test

ADD https://lhapdfsets.web.cern.ch/current/NNPDF31_nnlo_as_0118_luxqed.tar.gz /tmp/pdfset.tar.gz

RUN tar -xzf /tmp/pdfset.tar.gz -C /opt/lhapdf/share/LHAPDF && \
    cd /src/examples && \
    make -j"$(nproc)" main101 main121 main131 main164 main201 && \
    ./main101 && \
    ./main121 | grep 'useGzip = 1' && \
    ./main131 && \
    grep -q '^HepMC::Version 3.03.01' main131.hepmc && \
    ldd /opt/pythia/share/Pythia8/examples/main164 | grep -q libHepMC3 && \
    ./main164 -c main164mlm.cmnd && \
    ./main201

FROM dhi.io/debian-base:trixie-dev@sha256:af61a3296b0c92f16969f11ce8c2a93722e84c9e5c1bb42ed6afec796be71b04

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
ENV PATH=/opt/pythia/bin:/opt/hepmc/bin:/opt/lhapdf/bin:${PATH}
RUN echo /opt/hepmc/lib > /etc/ld.so.conf.d/hepmc.conf && \
    echo /opt/lhapdf/lib > /etc/ld.so.conf.d/lhapdf.conf && \
    echo /opt/pythia/lib > /etc/ld.so.conf.d/pythia.conf && \
    ldconfig && \
    echo /opt/lhapdf/lib/python3.13/dist-packages > /usr/lib/python3/dist-packages/lhapdf.pth

WORKDIR /root
CMD ["bash"]
