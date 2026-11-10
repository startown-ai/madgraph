FROM dhi.io/debian-base:trixie-dev@sha256:94571f3dc07fbc7166f6f1cbf1fe068a7647198ee2554e6b90d59a68cc8b053c AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      cmake=3.31.6-2 \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      gfortran=4:14.2.0-1+deb13u0+dhi0 \
      libboost-dev=1.83.0.2+b2 \
      ninja-build=1.12.1-1*

COPY --from=docker.io/startown/lhapdf:6.5.6-1@sha256:7f31495cb858a90951f4d06ff3d1cf952ec031c5345c9a548008ed6549eb2c9b /opt/lhapdf /opt/lhapdf
ENV PATH=/opt/lhapdf/bin:${PATH}
RUN echo /opt/lhapdf/lib > /etc/ld.so.conf.d/lhapdf.conf && \
    ldconfig

ADD --checksum=sha256:4495aaf754fb9da22d222f2a58761262beb012a907d6d2ec1d9b637c4f4743d2 https://assets.startown.ai/packages/emela/1.0.1.tar.gz /tmp/emela.tar.gz

RUN mkdir /src && \
    tar -xzf /tmp/emela.tar.gz -C /src --strip-components=1 && \
    cmake -S /src -B /build -G Ninja \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=/opt/emela \
      -DWITH_LHAPDF=ON && \
    cmake --build /build && \
    cmake --install /build

RUN install -Dm644 -t /opt/emela/share/licenses/emela /src/LICENSE

FROM scratch

COPY --from=build /opt/emela /opt/emela
