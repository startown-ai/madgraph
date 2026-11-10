FROM dhi.io/debian-base:trixie-dev@sha256:94571f3dc07fbc7166f6f1cbf1fe068a7647198ee2554e6b90d59a68cc8b053c AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      g++=4:14.2.0-1+deb13u0+dhi0 \
      libfreetype6=2.13.3+dfsg-1+deb13u1+dhi1 \
      make=4.4.1-2+deb13u0+dhi1 \
      patch=2.8-2+deb13u0+dhi3 \
      rsync=3.5.0+ds1-0+deb13u1+dhi1 \
      zlib1g-dev=1:1.3.dfsg+really1.3.1-1+deb13u1+dhi0

COPY --from=docker.io/startown/delphes:3.5.1-1@sha256:ef9473facad498c84128643287e266a09bc12138f70d52036e349b8dd95d0ee7 /opt/delphes /opt/delphes
COPY --from=docker.io/startown/hepmc:3.3.1-1@sha256:a7196e9d9031051d7d1dfd177fb4b1aad8ba8a7864d43f4bec186c3c682d0f3e /opt/hepmc /opt/hepmc
COPY --from=docker.io/startown/lhapdf:6.5.6-2@sha256:e428a4334e637e794682d3f7511ceff1fcbb269a002e3237eaa7981000c99e2c /opt/lhapdf /opt/lhapdf
COPY --from=docker.io/startown/root:6.36.14-1@sha256:95e00b7d3d196d4f3de275022d5718844f86bb03413741e25dee76e85c5ce746 /opt/root /opt/root
ENV PATH=/opt/root/bin:${PATH}
RUN printf '/opt/%s/lib\n' delphes hepmc lhapdf root > /etc/ld.so.conf.d/opt.conf && ldconfig

ADD --checksum=sha256:1ae551d14dac495ddfe6b344792035ebe410fe6c6004d44a335e0ece0e745adf https://assets.startown.ai/packages/pythia/8.317.tar.gz /tmp/pythia.tar.gz
COPY *.patch DelphesPythia8Event.h /tmp/

RUN mkdir /src && cd /src && \
    tar -xzf /tmp/pythia.tar.gz --strip-components=1 && \
    ./configure --prefix=/opt/pythia \
      --with-hepmc3=/opt/hepmc \
      --with-lhapdf6=/opt/lhapdf \
      --with-gzip && \
    make -j"$(nproc)" && \
    make install && \
    make -C /opt/pythia/share/Pythia8/examples main164
# root-config prints several flags, which must split into separate arguments
# hadolint ignore=SC2046
RUN patch -o /tmp/main164delphes.cc /opt/pythia/share/Pythia8/examples/main164.cc < /tmp/main164delphes.patch && \
    g++ /tmp/main164delphes.cc -o /opt/pythia/share/Pythia8/examples/main164delphes -O2 -std=c++17 -DGZIP -DHEPMC3 \
      -I/tmp -I/opt/pythia/include -I/opt/hepmc/include -I/opt/delphes/include $(root-config --cflags) \
      -L/opt/pythia/lib -Wl,-rpath,/opt/pythia/lib -lpythia8 \
      -L/opt/hepmc/lib -Wl,-rpath,/opt/hepmc/lib -lHepMC3 \
      -L/opt/delphes/lib -Wl,-rpath,/opt/delphes/lib -lDelphes \
      $(root-config --libs) -lEG -ldl -lz
RUN install -Dm644 -t /opt/pythia/share/licenses/pythia /src/COPYING
RUN rm -r /opt/pythia/lib/libpythia8.a \
      /opt/pythia/share/Pythia8/htmldoc \
      /opt/pythia/share/Pythia8/pdfdoc

FROM scratch

COPY --from=build /opt/pythia /opt/pythia
