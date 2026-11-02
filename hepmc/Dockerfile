FROM dhi.io/debian-base:trixie-dev@sha256:9e479c64f8d27f8fa368ef62804c2ddb9b3e43a68c13a200b2f668b4771b049e AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      cmake=3.31.6-2 \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      make=4.4.1-2+deb13u0+dhi1 \
      'ninja-build=1.12.1-1*' && \
    rm -rf /var/lib/apt/lists/*

ADD https://assets.startown.ai/packages/hepmc/3.3.1.tar.gz /tmp/hepmc.tgz

RUN mkdir /src && \
    tar -xzf /tmp/hepmc.tgz -C /src --strip-components=1 && \
    cmake -S /src -B /build -G Ninja \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=/opt/hepmc \
      -DCMAKE_INSTALL_LIBDIR=lib \
      -DHEPMC3_ENABLE_ROOTIO=OFF \
      -DHEPMC3_ENABLE_PYTHON=OFF && \
    cmake --build /build && \
    cmake --install /build && \
    install -D -m 644 -t /opt/hepmc/share/licenses/hepmc /src/COPYING /src/LICENCE

FROM build AS test

RUN cmake -S /src -B /build -DHEPMC3_ENABLE_TEST=ON && \
    cmake --build /build && \
    ctest --test-dir /build --output-on-failure

FROM dhi.io/debian-base:trixie-dev@sha256:9e479c64f8d27f8fa368ef62804c2ddb9b3e43a68c13a200b2f668b4771b049e

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      cmake=3.31.6-2 \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      make=4.4.1-2+deb13u0+dhi1 && \
    rm -rf /var/lib/apt/lists/*

COPY --from=build /opt/hepmc /opt/hepmc
ENV PATH=/opt/hepmc/bin:${PATH}
RUN echo /opt/hepmc/lib > /etc/ld.so.conf.d/hepmc.conf && \
    ldconfig

WORKDIR /root
CMD ["bash"]
