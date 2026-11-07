FROM dhi.io/debian-base:trixie-dev@sha256:61025cea33d5910165ced8b33edd3f6b6bfe161e129174887936a1b9a1442b20 AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      gcc=4:14.2.0-1+deb13u0+dhi0 \
      libbz2-dev=1.0.8-6+deb13u0+dhi2 \
      libexpat1-dev=2.9.0-1~deb13u1+dhi1 \
      libffi-dev=3.4.8-2+deb13u0+dhi2 \
      liblzma-dev=5.8.1-1+deb13u2+dhi0 \
      libncurses-dev=6.5+20250216-2+deb13u0+dhi5 \
      libreadline-dev=8.2-6+deb13u0+dhi2 \
      libsqlite3-dev=3.46.1-7+deb13u2+dhi3 \
      libssl-dev=3.5.7-1~deb13u3+dhi0 \
      make=4.4.1-2+deb13u0+dhi1 \
      patch=2.8-2+deb13u0+dhi3 \
      pkgconf=1.8.1-4+deb13u0+dhi1 \
      uuid-dev=2.41.5-0+deb13u1+dhi3 \
      zlib1g-dev=1:1.3.dfsg+really1.3.1-1+deb13u0+dhi4 && \
    rm -rf /var/lib/apt/lists/*

ADD https://assets.startown.ai/packages/python/3.13.18.tar.gz /tmp/python.tar.gz
COPY getaddrinfo-null-bytes.patch http-client-framing.patch /tmp/

RUN mkdir /src && \
    tar -xzf /tmp/python.tar.gz -C /src --strip-components=1 && \
    patch -d /src -p1 < /tmp/getaddrinfo-null-bytes.patch && \
    patch -d /src -p1 < /tmp/http-client-framing.patch && \
    cd /src && \
    ./configure \
      --prefix=/opt/python \
      --enable-shared \
      --enable-optimizations \
      --with-lto \
      --with-system-expat \
      --with-ensurepip=no \
      LDFLAGS=-Wl,-rpath,/opt/python/lib && \
    make -j$(nproc) && \
    make install && \
    rm -rf /opt/python/lib/python3.13/test && \
    install -D -m 644 -t /opt/python/share/licenses/python /src/LICENSE && \
    rm -rf /src

FROM scratch

COPY --from=build /opt/python /opt/python
