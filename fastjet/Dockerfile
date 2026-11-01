FROM dhi.io/debian-base:trixie-dev@sha256:9e479c64f8d27f8fa368ef62804c2ddb9b3e43a68c13a200b2f668b4771b049e AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      cmake=3.31.6-2 \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      make=4.4.1-2+deb13u0+dhi1 \
      ninja-build=1.12.1-1+b1 && \
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
    cmake --install /build/fastjet

RUN mkdir -p /src/fjcontrib && \
    tar -xzf /tmp/fjcontrib.tar.gz -C /src/fjcontrib --strip-components=1 && \
    cd /src/fjcontrib && \
    ./configure --fastjet-config=/opt/fastjet/bin/fastjet-config CXXFLAGS="-O3 -fPIC" && \
    make -j"$(nproc)" && \
    make install && \
    make fragile-shared && \
    make fragile-shared-install

FROM build AS test

RUN ctest --test-dir /build/fastjet --output-on-failure && \
    make -C /src/fjcontrib -j"$(nproc)" check

FROM dhi.io/debian-base:trixie-dev@sha256:9e479c64f8d27f8fa368ef62804c2ddb9b3e43a68c13a200b2f668b4771b049e

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      cmake=3.31.6-2 \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      make=4.4.1-2+deb13u0+dhi1 && \
    rm -rf /var/lib/apt/lists/*

COPY --from=build /opt/fastjet /opt/fastjet
ENV PATH=/opt/fastjet/bin:${PATH}
RUN echo /opt/fastjet/lib > /etc/ld.so.conf.d/fastjet.conf && \
    ldconfig

WORKDIR /root
CMD ["bash"]
