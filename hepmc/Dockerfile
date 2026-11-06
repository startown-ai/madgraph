FROM dhi.io/debian-base:trixie-dev@sha256:61025cea33d5910165ced8b33edd3f6b6bfe161e129174887936a1b9a1442b20 AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      cmake=3.31.6-2 \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      make=4.4.1-2+deb13u0+dhi1 \
      ninja-build=1.12.1-1* && \
    rm -rf /var/lib/apt/lists/*

ADD https://assets.startown.ai/packages/hepmc/3.3.1.tar.gz /tmp/hepmc.tar.gz

RUN mkdir /src && \
    tar -xzf /tmp/hepmc.tar.gz -C /src --strip-components=1 && \
    cmake -S /src -B /build -G Ninja \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=/opt/hepmc \
      -DCMAKE_INSTALL_LIBDIR=lib \
      -DHEPMC3_ENABLE_PYTHON=OFF \
      -DHEPMC3_ENABLE_ROOTIO=OFF && \
    cmake --build /build && \
    cmake --install /build && \
    install -D -m 644 -t /opt/hepmc/share/licenses/hepmc /src/COPYING /src/LICENCE && \
    rm -rf /src /build

FROM scratch

COPY --from=build /opt/hepmc /opt/hepmc
