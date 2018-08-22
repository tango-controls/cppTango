#!/usr/bin/env bash

docker exec cpp_tango mkdir -p /home/tango/cppzmq/build

echo "Build cppzmq"
docker exec cpp_tango cmake -H/home/tango/cppzmq -B/home/tango/cppzmq/build -DCMAKE_INSTALL_PREFIX=/home/tango
if [ $? -ne "0" ]
then
    exit -1
fi
echo "Install cppzmq"
docker exec cpp_tango make -C /home/tango/cppzmq/build install
if [ $? -ne "0" ]
then
    exit -1
fi