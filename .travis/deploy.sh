#!/usr/bin/env bash

echo "Package libtango"
docker exec cpp_tango make -C /home/tango/src/build package
if [ $? -ne "0" ]
then
    exit -1
fi

echo "Deploy libtango"
docker exec cpp_tango make -C /home/tango/src/build deploy
if [ $? -ne "0" ]
then
    exit -1
fi