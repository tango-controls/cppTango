#!/usr/bin/env bash

set -e

docker pull tangocs/mysql:9.2.2
docker pull tangocs/tango-cs:latest
git clone https://github.com/JoakimSoderberg/coveralls-cmake.git
(test ${STOCK_CPPZMQ} = "OFF" && git clone -b v4.2.2 https://${CI_USER_TOKEN}@github.com/zeromq/cppzmq.git cppzmq) || mkdir cppzmq
git clone -b tango-9-lts https://${CI_USER_TOKEN}@github.com/tango-controls/tango-idl.git idl
git clone -b Release_1.15 https://github.com/tango-controls/tango_admin.git
wget https://binaries.sonarsource.com/Distribution/sonar-scanner-cli/sonar-scanner-cli-4.1.0.1829-linux.zip && unzip sonar-scanner-cli-4.1.0.1829-linux.zip && mv sonar-scanner-4.1.0.1829-linux sonar-scanner
wget https://sonarcloud.io/static/cpp/build-wrapper-linux-x86.zip && unzip build-wrapper-linux-x86.zip
