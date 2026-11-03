FROM dhi.io/debian-base:trixie-dev@sha256:af61a3296b0c92f16969f11ce8c2a93722e84c9e5c1bb42ed6afec796be71b04 AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      make=4.4.1-2+deb13u0+dhi1 \
      python3-dev=3.13.5-1 && \
    rm -rf /var/lib/apt/lists/*

ADD https://assets.startown.ai/packages/lhapdf/6.5.6.tar.gz /tmp/lhapdf.tar.gz

RUN mkdir /src && \
    tar -xzf /tmp/lhapdf.tar.gz -C /src --strip-components=1 && \
    cd /src && \
    PYTHON=python3 ./configure --prefix=/opt/lhapdf --disable-doxygen && \
    make -j"$(nproc)" && \
    make install && \
    install -D -m 644 -t /opt/lhapdf/share/licenses/lhapdf /src/COPYING

FROM dhi.io/debian-base:trixie-dev@sha256:af61a3296b0c92f16969f11ce8c2a93722e84c9e5c1bb42ed6afec796be71b04

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      libpython3.13=3.13.5-2+deb13u5 \
      make=4.4.1-2+deb13u0+dhi1 \
      python3=3.13.5-1 && \
    rm -rf /var/lib/apt/lists/*

COPY --from=build /opt/lhapdf /opt/lhapdf
ENV PATH=/opt/lhapdf/bin:${PATH}
RUN echo /opt/lhapdf/lib > /etc/ld.so.conf.d/lhapdf.conf && \
    ldconfig && \
    echo /opt/lhapdf/lib/python3.13/dist-packages > /usr/lib/python3/dist-packages/lhapdf.pth

WORKDIR /root
CMD ["bash"]
