FROM debian:buster

ARG APP_UID=2000

ARG APP_GID=2000

MAINTAINER TANGO Controls team <tango@esrf.fr>

ENV DEBIAN_FRONTEND=noninteractive

RUN dpkg --add-architecture i386

# apt-get install needs two invocations
# to avoid an unreachable package error
RUN apt-get update && apt-get install -y \
  gcc-i686-linux-gnu                     \
  && apt-get install -y                  \
  apt-utils                              \
  build-essential                        \
  cmake                                  \
  curl                                   \
  g++-multilib                           \
  gcc-multilib                           \
  git                                    \
  libcos4-dev:i386                       \
  libomniorb4-dev:i386                   \
  libomnithread4-dev:i386                \
  libzmq3-dev:i386                       \
  lsb-release                            \
  omniidl                                \
  python3

RUN apt-get install -y \
    apt-transport-https \
    ca-certificates \
    curl \
    gnupg2 \
    software-properties-common \
 && curl -fsSL https://download.docker.com/linux/debian/gpg | apt-key add - \
 && add-apt-repository \
   "deb [arch=amd64] https://download.docker.com/linux/debian buster stable" \
 && apt-get update \
 && apt-get install -y docker-ce

RUN groupadd -g "$APP_GID" tango

RUN useradd -u "$APP_UID" -g "$APP_GID" -ms /bin/bash tango

RUN usermod -a -G docker tango

ENV PKG_CONFIG_PATH=/home/tango/lib/pkgconfig

USER tango

WORKDIR /home/tango
