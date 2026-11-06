FROM dhi.io/debian-base:trixie-dev@sha256:61025cea33d5910165ced8b33edd3f6b6bfe161e129174887936a1b9a1442b20 AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      cmake=3.31.6-2 \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      libftgl-dev=2.4.0-5* \
      libfreetype-dev=2.13.3+dfsg-1+deb13u1+dhi1 \
      libgif-dev=5.2.2-1+deb13u1 \
      libglew-dev=2.2.0-4* \
      libglu1-mesa-dev=9.0.2-1.1* \
      libjpeg-dev=1:2.1.5-4+deb13u0+dhi0 \
      liblz4-dev=1.10.0-4+deb13u0+dhi3 \
      liblzma-dev=5.8.1-1+deb13u2+dhi0 \
      libpcre2-dev=10.46-1~deb13u3+dhi0 \
      libssl-dev=3.5.7-1~deb13u3+dhi0 \
      libtbb-dev=2022.1.0-1+deb13u1 \
      libvdt-dev=0.4.4-4 \
      libx11-dev=2:1.8.12-1+dhi0 \
      libxext-dev=2:1.3.4-1+dhi0 \
      libxft-dev=2.3.6-1+dhi0 \
      libxpm-dev=1:3.5.17-1+deb13u1+dhi1 \
      libxxhash-dev=0.8.3-2+deb13u0+dhi1 \
      libzstd-dev=1.5.7+dfsg-1+deb13u0+dhi3 \
      make=4.4.1-2+deb13u0+dhi1 \
      ninja-build=1.12.1-1* \
      nlohmann-json3-dev=3.11.3-2.1 \
      python3-dev=3.13.5-1 \
      zlib1g-dev=1:1.3.dfsg+really1.3.1-1+deb13u0+dhi4 && \
    rm -rf /var/lib/apt/lists/*

COPY --from=root /opt/root /opt/root
ENV PATH=/opt/root/bin:${PATH}
RUN echo /opt/root/lib > /etc/ld.so.conf.d/root.conf && \
    ldconfig

ADD https://assets.startown.ai/packages/delphes/3.5.1.tar.gz /tmp/delphes.tar.gz

RUN mkdir /src && \
    tar -xzf /tmp/delphes.tar.gz -C /src --strip-components=1 && \
    cmake -S /src -B /build -G Ninja \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=/opt/delphes && \
    cmake --build /build && \
    cmake --install /build && \
    install -D -m 644 -t /opt/delphes/share/licenses/delphes /src/COPYING /src/LICENSE && \
    rm -rf /src /build

FROM scratch

COPY --from=build /opt/delphes /opt/delphes
