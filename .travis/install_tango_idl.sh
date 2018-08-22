#!/usr/bin/env bash

docker exec cpp_tango mkdir -p /home/tango/idl/build

echo "Build tango-idl"
docker exec cpp_tango cmake -H/home/tango/idl -B/home/tango/idl/build -DCMAKE_INSTALL_PREFIX=/home/tango
if [ $? -ne "0" ]
then
    exit -1
fi
echo "Install tango-idl"
docker exec cpp_tango make -C /home/tango/idl/build install
if [ $? -ne "0" ]
then
    exit -1
fi