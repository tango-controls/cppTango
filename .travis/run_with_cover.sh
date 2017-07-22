#!/usr/bin/env bash
#this script is executed from within Travis environment. TANGO_HOST is already set
echo "TANGO_HOST=$TANGO_HOST"

docker exec cpp_tango mkdir -p /home/tango/src/build

echo "Run cmake cppTango in $CMAKE_BUILD_TYPE mode"
echo "Using COVERALLS=$COVERALLS"
docker exec cpp_tango cmake -H/home/tango/src -B/home/tango/src/build -DCOVERALLS=$COVERALLS -DCOVERALLS_MODULE_PATH=/home/tango/coveralls-cmake/cmake -DCMAKE_VERBOSE_MAKEFILE=true -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE
if [ $? -ne "0" ]
then
    exit -1
fi
echo "Run make cppTango"
docker exec cpp_tango make -C /home/tango/src/build -j 2
if [ $? -ne "0" ]
then
    exit -1
fi
echo "Run pre_test"
docker exec cpp_tango /bin/sh -c 'cd /home/tango/src/build/test/cpp_test_suite/environment; exec ./pre_test.sh'
if [ $? -ne "0" ]
then
    exit -1
fi
echo "Run tests"
docker exec cpp_tango /bin/sh -c 'cd /home/tango/src/build; exec make coveralls'
if [ $? -ne "0" ]
then
    exit -1
fi