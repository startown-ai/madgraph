FROM dhi.io/debian-base:trixie-dev@sha256:af61a3296b0c92f16969f11ce8c2a93722e84c9e5c1bb42ed6afec796be71b04 AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      cmake=3.31.6-2 \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      make=4.4.1-2+deb13u0+dhi1 \
      'ninja-build=1.12.1-1*' && \
    rm -rf /var/lib/apt/lists/*

ADD https://assets.startown.ai/packages/expat/2.10.0.tar.gz /tmp/expat.tar.gz

RUN mkdir /src && \
    tar -xzf /tmp/expat.tar.gz -C /src --strip-components=1 && \
    cmake -S /src -B /build -G Ninja \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=/opt/expat \
      -DCMAKE_INSTALL_LIBDIR=lib \
      -DEXPAT_BUILD_TOOLS=OFF \
      -DEXPAT_BUILD_EXAMPLES=OFF \
      -DEXPAT_BUILD_TESTS=OFF \
      -DEXPAT_BUILD_DOCS=OFF && \
    cmake --build /build && \
    cmake --install /build && \
    install -D -m 644 -t /opt/expat/share/licenses/expat /src/COPYING

FROM scratch

COPY --from=build /opt/expat /opt/expat
