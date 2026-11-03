FROM dhi.io/debian-base:trixie-dev@sha256:af61a3296b0c92f16969f11ce8c2a93722e84c9e5c1bb42ed6afec796be71b04 AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      cmake=3.31.6-2 \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      git=1:2.47.3-0+deb13u1+dhi2 \
      libfftw3-dev=3.3.10-2+dhi0 \
      libfreetype-dev=2.13.3+dfsg-1+deb13u1+dhi1 \
      libgmock-dev=1.16.0-1 \
      libgtest-dev=1.16.0-1 \
      liblz4-dev=1.10.0-4+deb13u0+dhi3 \
      liblzma-dev=5.8.1-1+deb13u2+dhi0 \
      libpcre2-dev=10.46-1~deb13u3+dhi0 \
      libssl-dev=3.5.7-1~deb13u3+dhi0 \
      libtbb-dev=2022.1.0-1+deb13u1 \
      libvdt-dev=0.4.4-4 \
      libxml2-dev=2.12.7+dfsg+really2.9.14-2.1+deb13u3+dhi3 \
      libxxhash-dev=0.8.3-2+deb13u0+dhi1 \
      libzstd-dev=1.5.7+dfsg-1+deb13u0+dhi3 \
      make=4.4.1-2+deb13u0+dhi1 \
      'ninja-build=1.12.1-1*' \
      nlohmann-json3-dev=3.11.3-2.1 \
      python3-dev=3.13.5-1 \
      uuid-dev=2.41.5-0+deb13u1+dhi3 \
      zlib1g-dev=1:1.3.dfsg+really1.3.1-1+deb13u0+dhi4 && \
    rm -rf /var/lib/apt/lists/*

ADD https://assets.startown.ai/packages/root/6.40.04.tar.gz /tmp/root.tar.gz

RUN mkdir /src && \
    tar -xzf /tmp/root.tar.gz -C /src --strip-components=1 && \
    cmake -S /src -B /build -G Ninja \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=/opt/root \
      -Dfail-on-missing=ON \
      -Dfftw3=ON \
      -Dx11=OFF \
      -Dopengl=OFF \
      -Dasimage=OFF \
      -Dcurl=OFF \
      -Ddavix=OFF \
      -Dfitsio=OFF \
      -Dsqlite=OFF \
      -Dtmva-cpu=OFF \
      -Dxrootd=OFF \
      -Dtesting=ON && \
    cmake --build /build && \
    cmake --install /build

FROM build AS test

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      python3-numpy=1:2.2.4+ds-1 && \
    rm -rf /var/lib/apt/lists/*

RUN ctest --test-dir /build -j"$(nproc)" --output-on-failure \
      -E 'stressgeometry|df014_CSVDataSource|df015_LazyDataSource|tmva100_DataPreparation|exampleFunction'

FROM dhi.io/debian-base:trixie-dev@sha256:af61a3296b0c92f16969f11ce8c2a93722e84c9e5c1bb42ed6afec796be71b04

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      libfftw3-double3=3.3.10-2+dhi0 \
      libfreetype6=2.13.3+dfsg-1+deb13u1+dhi1 \
      libpython3.13=3.13.5-2+deb13u5 \
      libtbb12=2022.1.0-1+deb13u1 \
      libvdt-dev=0.4.4-4 \
      libxml2=2.12.7+dfsg+really2.9.14-2.1+deb13u3+dhi3 \
      make=4.4.1-2+deb13u0+dhi1 \
      python3=3.13.5-1 \
      python3-numpy=1:2.2.4+ds-1 && \
    rm -rf /var/lib/apt/lists/*

COPY --from=build /opt/root /opt/root
ENV PATH=/opt/root/bin:${PATH}
RUN echo /opt/root/lib > /etc/ld.so.conf.d/root.conf && \
    ldconfig && \
    echo /opt/root/lib > /usr/lib/python3/dist-packages/root.pth

WORKDIR /root
CMD ["bash"]
