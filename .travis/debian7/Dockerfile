FROM debian:wheezy-backports

MAINTAINER TANGO Controls team <tango@esrf.fr>

RUN apt-get update && apt-get install -y apt-utils

RUN apt-get update && apt-get install -y build-essential cmake

RUN apt-get update && apt-get install -y omniidl libomniorb4-dev libcos4-dev libomnithread3-dev libzmq3-dev

RUN useradd -ms /bin/bash tango

ENV PKG_CONFIG_PATH=/home/tango/lib/pkgconfig

USER tango

WORKDIR /home/tango