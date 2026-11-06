FROM dhi.io/debian-base:trixie-dev@sha256:61025cea33d5910165ced8b33edd3f6b6bfe161e129174887936a1b9a1442b20 AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      make=4.4.1-2+deb13u0+dhi1 \
      python3-dev=3.13.5-1 && \
    rm -rf /var/lib/apt/lists/*

ADD https://assets.startown.ai/packages/lhapdf/6.5.6.tar.gz /tmp/lhapdf.tar.gz

RUN mkdir /src && \
    tar -xzf /tmp/lhapdf.tar.gz -C /src --strip-components=1 && \
    cd /src && \
    PYTHON=python3 ./configure --prefix=/opt/lhapdf --disable-doxygen && \
    make -j$(nproc) && \
    make install && \
    install -D -m 644 -t /opt/lhapdf/share/licenses/lhapdf /src/COPYING && \
    rm -rf /src

FROM scratch

COPY --from=build /opt/lhapdf /opt/lhapdf
