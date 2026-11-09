FROM dhi.io/debian-base:trixie-dev@sha256:94571f3dc07fbc7166f6f1cbf1fe068a7647198ee2554e6b90d59a68cc8b053c AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      make=4.4.1-2+deb13u0+dhi1 \
      rsync=3.5.0+ds1-0+deb13u1+dhi1 \
      zlib1g-dev=1:1.3.dfsg+really1.3.1-1+deb13u1+dhi0

COPY --from=hepmc /opt/hepmc /opt/hepmc
COPY --from=lhapdf /opt/lhapdf /opt/lhapdf
RUN echo /opt/hepmc/lib > /etc/ld.so.conf.d/hepmc.conf && \
    echo /opt/lhapdf/lib > /etc/ld.so.conf.d/lhapdf.conf && \
    ldconfig

ADD https://assets.startown.ai/packages/pythia/8.317.tar.gz /tmp/pythia.tar.gz

RUN mkdir /src && cd /src && \
    tar -xzf /tmp/pythia.tar.gz --strip-components=1 && \
    ./configure --prefix=/opt/pythia \
      --with-hepmc3=/opt/hepmc \
      --with-lhapdf6=/opt/lhapdf \
      --with-gzip && \
    make -j"$(nproc)" && \
    make install && \
    make -C /opt/pythia/share/Pythia8/examples main164

RUN install -Dm644 -t /opt/pythia/share/licenses/pythia /src/COPYING && \
    rm -r /opt/pythia/lib/libpythia8.a \
      /opt/pythia/share/Pythia8/htmldoc \
      /opt/pythia/share/Pythia8/pdfdoc

FROM scratch

COPY --from=build /opt/pythia /opt/pythia
