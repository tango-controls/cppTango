FROM debian/eol:wheezy

ARG APP_UID=2000

ARG APP_GID=2000

MAINTAINER TANGO Controls team <tango@esrf.fr>

RUN echo 'deb http://archive.debian.org/debian wheezy-backports main contrib' > /etc/apt/sources.list.d/backports.list

RUN echo "Acquire::Check-Valid-Until false;" > /etc/apt/apt.conf

RUN apt-get update && apt-get install -y \
  apt-utils                              \
  build-essential                        \
  curl                                   \
  git                                    \
  libcos4-dev                            \
  libomniorb4-dev                        \
  libomnithread3-dev                     \
  libzmq3-dev                            \
  lsb-release                            \
  omniidl                                \
  procps                                 \
  wget

RUN wget --no-check-certificate https://cmake.org/files/v3.10/cmake-3.10.0-Linux-x86_64.sh -O /tmp/cmake-install.sh \
  && chmod +x /tmp/cmake-install.sh                                                                                 \
  && /tmp/cmake-install.sh --skip-license --exclude-subdir

RUN groupadd -g "$APP_GID" tango

RUN useradd -u "$APP_UID" -g "$APP_GID" -ms /bin/bash tango

ENV PKG_CONFIG_PATH=/home/tango/lib/pkgconfig

USER tango

WORKDIR /home/tango
