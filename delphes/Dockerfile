FROM dhi.io/debian-base:trixie-dev@sha256:af61a3296b0c92f16969f11ce8c2a93722e84c9e5c1bb42ed6afec796be71b04 AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      cmake=3.31.6-2 \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      libfftw3-double3=3.3.10-2+dhi0 \
      libfreetype6=2.13.3+dfsg-1+deb13u1+dhi1 \
      libpython3.13=3.13.5-2+deb13u5 \
      libtbb12=2022.1.0-1+deb13u1 \
      libvdt-dev=0.4.4-4 \
      libxml2=2.12.7+dfsg+really2.9.14-2.1+deb13u3+dhi3 \
      make=4.4.1-2+deb13u0+dhi1 \
      'ninja-build=1.12.1-1*' \
      patch=2.8-2+deb13u0+dhi3 \
      python3=3.13.5-1 && \
    rm -rf /var/lib/apt/lists/*

COPY --from=root /opt/root /opt/root
ENV PATH=/opt/root/bin:${PATH}
RUN echo /opt/root/lib > /etc/ld.so.conf.d/root.conf && \
    ldconfig

ADD https://assets.startown.ai/packages/delphes/3.5.1.tar.gz /tmp/delphes.tar.gz
COPY no-display.patch /tmp/no-display.patch

RUN mkdir /src && \
    tar -xzf /tmp/delphes.tar.gz -C /src --strip-components=1 && \
    patch -d /src -p1 < /tmp/no-display.patch && \
    cmake -S /src -B /build -G Ninja \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=/opt/delphes && \
    cmake --build /build && \
    cmake --install /build && \
    ln -s . /opt/delphes/include/external && \
    find /build -name '*.rootmap' -exec cat {} + | \
      sed 's/^\[ lib.*Dict\.so \]$/[ libDelphes.so ]/' > /opt/delphes/lib/libDelphes.rootmap && \
    install -D -m 644 -t /opt/delphes/share/licenses/delphes /src/COPYING /src/LICENSE

FROM scratch

COPY --from=build /opt/delphes /opt/delphes
