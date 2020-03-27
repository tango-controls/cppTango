FROM debian:jessie

ARG APP_UID=2000

ARG APP_GID=2000

MAINTAINER TANGO Controls team <tango@esrf.fr>

ENV DEBIAN_FRONTEND=noninteractive

RUN sed -i '/jessie-updates/d' /etc/apt/sources.list  # Now archived

RUN apt-get update && apt-get install -y \
  apt-utils                              \
  build-essential                        \
  curl                                   \
  git                                    \
  lsb-release                            \
  omniidl                                \
  libomniorb4-dev                        \
  libcos4-dev                            \
  libomnithread3-dev                     \
  libzmq3-dev                            \
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
 && curl -fsSL https://download.docker.com/linux/debian/gpg | apt-key add - \
 && add-apt-repository \
   "deb [arch=amd64] https://download.docker.com/linux/debian jessie stable" \
 && apt-get update \
 && apt-get install -y docker-ce

RUN groupadd -g "$APP_GID" tango

RUN useradd -u "$APP_UID" -g "$APP_GID" -ms /bin/bash tango

RUN usermod -a -G docker tango

ENV PKG_CONFIG_PATH=/home/tango/lib/pkgconfig

USER tango

WORKDIR /home/tango
