FROM dhi.io/debian-base:trixie-dev@sha256:61025cea33d5910165ced8b33edd3f6b6bfe161e129174887936a1b9a1442b20 AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      make=4.4.1-2+deb13u0+dhi1 \
      zlib1g-dev=1:1.3.dfsg+really1.3.1-1+deb13u0+dhi4 && \
    rm -rf /var/lib/apt/lists/*

ADD https://assets.startown.ai/packages/binutils/2.47.tar.gz /tmp/binutils.tar.gz

RUN mkdir /src /build && \
    tar -xzf /tmp/binutils.tar.gz -C /src --strip-components=1 && \
    triplet=$(gcc -dumpmachine) && \
    cd /build && \
    /src/configure \
      --prefix=/opt/binutils \
      --build=$triplet \
      --with-lib-path=/usr/local/lib/$triplet:/lib/$triplet:/usr/lib/$triplet:/usr/local/lib:/lib:/usr/lib \
      --with-sysroot=/ \
      --enable-plugins \
      --enable-new-dtags \
      --enable-deterministic-archives \
      --enable-gold=no \
      --disable-gprofng \
      --disable-nls \
      --disable-shared \
      --disable-werror \
      --with-system-zlib \
      --without-zstd \
      MAKEINFO=true && \
    make -j$(nproc) MAKEINFO=true && \
    make install MAKEINFO=true && \
    rm -rf /opt/binutils/share/info /opt/binutils/share/man && \
    install -D -m 644 -t /opt/binutils/share/licenses/binutils \
      /src/COPYING /src/COPYING3 /src/COPYING.LIB /src/COPYING3.LIB && \
    rm -rf /src /build

FROM scratch

COPY --from=build /opt/binutils /opt/binutils
