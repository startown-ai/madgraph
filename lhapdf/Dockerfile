FROM dhi.io/debian-base:trixie-dev@sha256:94571f3dc07fbc7166f6f1cbf1fe068a7647198ee2554e6b90d59a68cc8b053c AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      make=4.4.1-2+deb13u0+dhi1 \
      python-3.13-dev=3.13.18-6

ADD --checksum=sha256:6b8b7e38dc26a977a24f5a321215b7054c14a4469d04134d70cb93a860eeeea7 https://assets.startown.ai/packages/lhapdf/6.5.6.tar.gz /tmp/lhapdf.tar.gz

RUN ln -s "$(gcc -dumpmachine)/libpython3.13.so" /usr/lib/libpython3.13.so
ENV PYTHON=python3

RUN mkdir /src && cd /src && \
    tar -xzf /tmp/lhapdf.tar.gz --strip-components=1 && \
    ./configure --prefix=/opt/lhapdf \
      --disable-doxygen \
      --disable-static && \
    make -j"$(nproc)" && \
    make install

RUN install -Dm644 -t /opt/lhapdf/share/licenses/lhapdf /src/COPYING

FROM scratch

COPY --from=build /opt/lhapdf /opt/lhapdf
