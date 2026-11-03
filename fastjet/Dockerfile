FROM dhi.io/debian-base:trixie-dev@sha256:af61a3296b0c92f16969f11ce8c2a93722e84c9e5c1bb42ed6afec796be71b04 AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      cmake=3.31.6-2 \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      make=4.4.1-2+deb13u0+dhi1 \
      'ninja-build=1.12.1-1*' && \
    rm -rf /var/lib/apt/lists/*

ADD https://assets.startown.ai/packages/fastjet/3.5.3.tar.gz /tmp/fastjet.tar.gz
ADD https://assets.startown.ai/packages/fjcontrib/1.105.tar.gz /tmp/fjcontrib.tar.gz

RUN mkdir -p /src/fastjet && \
    tar -xzf /tmp/fastjet.tar.gz -C /src/fastjet --strip-components=1 && \
    cmake -S /src/fastjet -B /build/fastjet -G Ninja \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=/opt/fastjet \
      -DFASTJET_ENABLE_ALLCXXPLUGINS=ON && \
    cmake --build /build/fastjet && \
    cmake --install /build/fastjet && \
    install -D -m 644 -t /opt/fastjet/share/licenses/fastjet /src/fastjet/COPYING

RUN mkdir -p /src/fjcontrib && \
    tar -xzf /tmp/fjcontrib.tar.gz -C /src/fjcontrib --strip-components=1 && \
    cd /src/fjcontrib && \
    ./configure --fastjet-config=/opt/fastjet/bin/fastjet-config CXXFLAGS="-O3 -fPIC" && \
    make -j"$(nproc)" && \
    make install && \
    make fragile-shared && \
    make fragile-shared-install && \
    find . -name COPYING -not -path './scripts/*' \
      -exec install -D -m 644 {} /opt/fastjet/share/licenses/fjcontrib/{} \;

FROM scratch

COPY --from=build /opt/fastjet /opt/fastjet
