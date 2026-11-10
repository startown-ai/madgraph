FROM dhi.io/debian-base:trixie-dev@sha256:94571f3dc07fbc7166f6f1cbf1fe068a7647198ee2554e6b90d59a68cc8b053c AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      make=4.4.1-2+deb13u0+dhi1 \
      patch=2.8-2+deb13u0+dhi3 \
      python-3.13-dev=3.13.18-7

ADD --checksum=sha256:6b8b7e38dc26a977a24f5a321215b7054c14a4469d04134d70cb93a860eeeea7 https://assets.startown.ai/packages/lhapdf/6.5.6.tar.gz /tmp/lhapdf.tar.gz
ADD --checksum=sha256:60d3c1df1c31e5840f91f4217163ae30a256b9291a5adc894882e86607ef5d63 https://assets.startown.ai/packages/lhapdf-sets/NNPDF23_lo_as_0130_qed.tar.gz /tmp/pdfset.tar.gz
COPY *.patch /tmp/

RUN mkdir /src && cd /src && \
    tar -xzf /tmp/lhapdf.tar.gz --strip-components=1 && \
    patch -d /src -p1 < /tmp/multiarch-libpython.patch && \
    ./configure --prefix=/opt/lhapdf \
      --disable-doxygen \
      --disable-static && \
    make -j"$(nproc)" && \
    make install
RUN tar -xzf /tmp/pdfset.tar.gz -C /opt/lhapdf/share/LHAPDF
RUN install -Dm644 -t /opt/lhapdf/share/licenses/lhapdf /src/COPYING

FROM scratch

COPY --from=build /opt/lhapdf /opt/lhapdf
