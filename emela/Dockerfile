FROM dhi.io/debian-base:trixie-dev@sha256:94571f3dc07fbc7166f6f1cbf1fe068a7647198ee2554e6b90d59a68cc8b053c AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      cmake=3.31.6-2 \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      gfortran=4:14.2.0-1+deb13u0+dhi0 \
      libboost-dev=1.83.0.2* \
      make=4.4.1-2+deb13u0+dhi1 \
      ninja-build=1.12.1-1*

COPY --from=lhapdf /opt/lhapdf /opt/lhapdf
ENV PATH=/opt/lhapdf/bin:${PATH}
RUN echo /opt/lhapdf/lib > /etc/ld.so.conf.d/lhapdf.conf && \
    ldconfig

ADD https://assets.startown.ai/packages/emela/1.0.1.tar.gz /tmp/emela.tar.gz

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
