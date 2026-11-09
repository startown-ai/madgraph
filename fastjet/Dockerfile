FROM dhi.io/debian-base:trixie-dev@sha256:94571f3dc07fbc7166f6f1cbf1fe068a7647198ee2554e6b90d59a68cc8b053c AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      cmake=3.31.6-2 \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      make=4.4.1-2+deb13u0+dhi1 \
      ninja-build=1.12.1-1*

ADD https://assets.startown.ai/packages/fastjet/3.5.3.tar.gz /tmp/fastjet.tar.gz

RUN mkdir /src && \
    tar -xzf /tmp/fastjet.tar.gz -C /src --strip-components=1 && \
    cmake -S /src -B /build -G Ninja \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=/opt/fastjet \
      -DFASTJET_BUILD_EXAMPLES=OFF \
      -DFASTJET_ENABLE_ALLCXXPLUGINS=ON \
      -DFASTJET_ENABLE_DEBUG=OFF \
      -DSISCONE_BUILD_EXAMPLES=OFF \
      -DSISCONE_ENABLE_DEBUG=OFF && \
    cmake --build /build && \
    cmake --install /build

RUN install -Dm644 -t /opt/fastjet/share/licenses/fastjet /src/COPYING

FROM scratch

COPY --from=build /opt/fastjet /opt/fastjet
