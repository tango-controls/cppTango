#!/usr/bin/env bash

set -e

if [[ "$RUN_TESTS" == "ON" ]]
then
  docker pull tangocs/mysql:9.2.2
  docker pull tangocs/tango-cs:latest
  git clone -b Release_1.15 --depth 1 https://github.com/tango-controls/tango_admin.git
else
  mkdir tango_admin
fi

if [[ "$SONAR_SCANNER" == "ON" ||  "$COVERALLS" == "ON" ]]
then
  git clone --depth 1 https://github.com/JoakimSoderberg/coveralls-cmake.git
fi

if [[ "$STOCK_CPPZMQ" == "OFF" ]]
then
  git clone -b v4.2.2 --depth 1 https://github.com/zeromq/cppzmq.git
else
  mkdir cppzmq
fi

git clone -b tango-9-lts --depth 1 https://github.com/tango-controls/tango-idl.git idl

if [[ "$SONAR_SCANNER" == "ON" ]]
then
  wget https://binaries.sonarsource.com/Distribution/sonar-scanner-cli/sonar-scanner-cli-4.1.0.1829-linux.zip
  unzip sonar-scanner-cli-4.1.0.1829-linux.zip
  mv sonar-scanner-4.1.0.1829-linux sonar-scanner
  wget https://sonarcloud.io/static/cpp/build-wrapper-linux-x86.zip
  unzip build-wrapper-linux-x86.zip
fi
