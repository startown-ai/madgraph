FROM dhi.io/debian-base:trixie-dev@sha256:94571f3dc07fbc7166f6f1cbf1fe068a7647198ee2554e6b90d59a68cc8b053c AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      cmake=3.31.6-2 \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      ninja-build=1.12.1-1*

ADD --checksum=sha256:08240160b0f28dc3293aa4d61ce65e2d67cd597acf6faca439f2e46625f7e793 https://assets.startown.ai/packages/hepmc/3.3.1.tar.gz /tmp/hepmc.tar.gz

RUN mkdir /src && \
    tar -xzf /tmp/hepmc.tar.gz -C /src --strip-components=1 && \
    cmake -S /src -B /build -G Ninja \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=/opt/hepmc \
      -DCMAKE_INSTALL_LIBDIR=lib \
      -DHEPMC3_BUILD_STATIC_LIBS=OFF \
      -DHEPMC3_ENABLE_PYTHON=OFF \
      -DHEPMC3_ENABLE_ROOTIO=OFF \
      -DHEPMC3_ENABLE_SEARCH=OFF \
      -DHEPMC3_INSTALL_EXAMPLES=OFF && \
    cmake --build /build && \
    cmake --install /build

RUN install -Dm644 -t /opt/hepmc/share/licenses/hepmc /src/COPYING /src/LICENCE

FROM scratch

COPY --from=build /opt/hepmc /opt/hepmc
