FROM dhi.io/debian-base:trixie-dev@sha256:af61a3296b0c92f16969f11ce8c2a93722e84c9e5c1bb42ed6afec796be71b04 AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      cmake=3.31.6-2 \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      gfortran=4:14.2.0-1+deb13u0+dhi0 \
      libboost-dev=1.83.0.2+b2 \
      make=4.4.1-2+deb13u0+dhi1 \
      'ninja-build=1.12.1-1*' && \
    rm -rf /var/lib/apt/lists/*

COPY --from=startown/lhapdf:6.5.6@sha256:d4017b0b918a2107d10331933b13096664e9ce5f6b4b039bb43e96f71ef7272f /opt/lhapdf /opt/lhapdf
ENV PATH=/opt/lhapdf/bin:${PATH}
RUN echo /opt/lhapdf/lib > /etc/ld.so.conf.d/lhapdf.conf && \
    ldconfig

ADD https://assets.startown.ai/packages/emela/1.0.1.tar.gz /tmp/emela.tar.gz

RUN mkdir /src && \
    tar -xzf /tmp/emela.tar.gz -C /src --strip-components=1 && \
    cmake -S /src -B /build -G Ninja \
      -DCMAKE_INSTALL_PREFIX=/opt/emela \
      -DWITH_LHAPDF=ON && \
    cmake --build /build && \
    cmake --install /build && \
    install -D -m 644 -t /opt/emela/share/licenses/emela /src/LICENSE

FROM scratch

COPY --from=build /opt/emela /opt/emela
