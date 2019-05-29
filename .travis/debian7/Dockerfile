FROM debian/eol:wheezy

ARG APP_UID=2000

ARG APP_GID=2000

MAINTAINER TANGO Controls team <tango@esrf.fr>

RUN echo 'deb http://archive.debian.org/debian wheezy-backports main contrib' > /etc/apt/sources.list.d/backports.list

RUN echo "Acquire::Check-Valid-Until false;" > /etc/apt/apt.conf

RUN apt-get update && apt-get install -y apt-utils

RUN apt-get install -y build-essential

RUN apt-get install -y procps

RUN apt-get install -y wget

RUN apt-get install -y curl lsb-release

RUN wget --no-check-certificate https://cmake.org/files/v3.10/cmake-3.10.0-Linux-x86_64.sh -O /tmp/cmake-install.sh

RUN chmod +x /tmp/cmake-install.sh

RUN /tmp/cmake-install.sh --skip-license --exclude-subdir

RUN apt-get install -y omniidl libomniorb4-dev libcos4-dev libomnithread3-dev libzmq3-dev

RUN groupadd -g "$APP_GID" tango

RUN useradd -u "$APP_UID" -g "$APP_GID" -ms /bin/bash tango

ENV PKG_CONFIG_PATH=/home/tango/lib/pkgconfig

USER tango

WORKDIR /home/tango
