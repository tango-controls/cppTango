#!/usr/bin/env bash
#this script is executed from within Travis environment. TANGO_HOST is already set
echo "TANGO_HOST=$TANGO_HOST"

docker exec cpp_tango mkdir -p /home/tango/src/build

echo "Run cmake cppTango in $CMAKE_BUILD_TYPE mode"
echo "Using COVERALLS=$COVERALLS"

TEST_COMMAND="exec ctest -V"
if [ $COVERALLS = "ON" ]
then
    TEST_COMMAND="exec make coveralls"
fi

docker exec cpp_tango cmake -H/home/tango/src -B/home/tango/src/build -DCOVERALLS=$COVERALLS -DCOVERALLS_MODULE_PATH=/home/tango/coveralls-cmake/cmake -DCMAKE_VERBOSE_MAKEFILE=true -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE
if [ $? -ne "0" ]
then
    exit -1
fi
docker exec cpp_tango make -C /home/tango/src/build -j 2
if [ $? -ne "0" ]
then
    exit -1
fi

docker exec cpp_tango make -C /home/tango/src/build -j 2
echo "Test log4tango"
docker exec cpp_tango /bin/sh -c 'cd /home/tango/src/build/log4tango; exec ctest -V'
if [ $? -ne "0" ]
then
    exit -1
fi
echo "Test cppTango"
echo "TEST_COMMAND=$TEST_COMMAND"
docker exec cpp_tango /bin/sh -c "cd /home/tango/src/build/cpp_test_suite; $TEST_COMMAND"
if [ $? -ne "0" ]
then
    exit -1
fi