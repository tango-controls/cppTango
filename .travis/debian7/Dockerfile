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

RUN apt-get install -y \
    apt-transport-https \
    ca-certificates \
    curl \
    gnupg2 \
    software-properties-common \
    python-software-properties \
 && curl -fsSL https://download.docker.com/linux/debian/gpg | apt-key add - \
 && add-apt-repository \
   "deb [arch=amd64] https://download.docker.com/linux/debian wheezy stable" \
 && sed -i -e '/^deb-src.*docker.*/d' /etc/apt/sources.list \
 && apt-get update \
 && apt-get install -y docker-ce

RUN groupadd -g "$APP_GID" tango

RUN useradd -u "$APP_UID" -g "$APP_GID" -ms /bin/bash tango

RUN usermod -a -G docker tango

ENV PKG_CONFIG_PATH=/home/tango/lib/pkgconfig

USER tango

WORKDIR /home/tango
