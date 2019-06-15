FROM debian:stretch

ARG APP_UID=2000

ARG APP_GID=2000

MAINTAINER TANGO Controls team <tango@esrf.fr>

RUN apt-get update && apt-get install -y apt-utils

RUN apt-get install -y build-essential cmake

RUN apt-get install -y curl lsb-release

RUN apt-get install -y omniidl libomniorb4-dev libcos4-dev libomnithread3-dev libzmq3-dev

RUN apt-get install -y \
    apt-transport-https \
    ca-certificates \
    curl \
    gnupg2 \
    software-properties-common \
 && curl -fsSL https://download.docker.com/linux/debian/gpg | apt-key add - \
 && add-apt-repository \
   "deb [arch=amd64] https://download.docker.com/linux/debian stretch stable" \
 && apt-get update \
 && apt-get install -y docker-ce

RUN groupadd -g "$APP_GID" tango

RUN useradd -u "$APP_UID" -g "$APP_GID" -ms /bin/bash tango

RUN usermod -a -G docker tango

ENV PKG_CONFIG_PATH=/home/tango/lib/pkgconfig

USER tango

WORKDIR /home/tango
