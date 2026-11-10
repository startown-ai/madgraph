FROM dhi.io/debian-base:trixie-dev@sha256:94571f3dc07fbc7166f6f1cbf1fe068a7647198ee2554e6b90d59a68cc8b053c AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      cmake=3.31.6-2 \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      libedit-dev=3.1-20250104-1+deb13u0+dhi2 \
      libfreetype-dev=2.13.3+dfsg-1+deb13u1+dhi1 \
      liblz4-dev=1.10.0-4+deb13u0+dhi3 \
      liblzma-dev=5.8.1-1+deb13u2+dhi0 \
      libpcre2-dev=10.46-1~deb13u3+dhi0 \
      libpolly-18-dev=1:18.1.8-18* \
      libxxhash-dev=0.8.3-2+deb13u0+dhi1 \
      libzstd-dev=1.5.7+dfsg-1+deb13u0+dhi3 \
      llvm-18-dev=1:18.1.8-18* \
      ninja-build=1.12.1-1* \
      nlohmann-json3-dev=3.11.3-2.1 \
      patch=2.8-2+deb13u0+dhi3 \
      python-3.13=3.13.18-6 \
      zlib1g-dev=1:1.3.dfsg+really1.3.1-1+deb13u1+dhi0

ADD --checksum=sha256:d45af0b98c121a1f03ad7f6069ef660da758d7eea519e24ad788768d58805284 https://assets.startown.ai/packages/root/6.36.14.tar.gz /tmp/root.tar.gz
COPY eg-no-graphics.patch /tmp/eg-no-graphics.patch

RUN mkdir /src && \
    tar -xzf /tmp/root.tar.gz -C /src --strip-components=1 && \
    patch -d /src -p1 < /tmp/eg-no-graphics.patch && \
    cmake -S /src -B /build -G Ninja \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=/opt/root \
      -Dfail-on-missing=ON \
      -Dminimal=ON \
      -Dbuiltin_llvm=OFF \
      -DLLVM_DIR=/usr/lib/llvm-18/lib/cmake/llvm \
      -DLLVM_INCLUDE_TESTS=OFF && \
    cmake --build /build && \
    cmake --install /build

RUN install -Dm644 -t /opt/root/share/licenses/root /src/LICENSE && \
    rm -r /opt/root/fonts \
      /opt/root/README/ReleaseNotes \
      /opt/root/tutorials

FROM scratch

COPY --from=build /opt/root /opt/root
