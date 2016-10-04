#!/usr/bin/env bash
docker exec cpp_tango mkdir -p /home/tango/idl/build
docker exec cpp_tango mkdir -p /home/tango/src/build

echo "Build tango-idl"
docker exec cpp_tango cmake -H/home/tango/idl -B/home/tango/idl/build -DCMAKE_INSTALL_PREFIX=/home/tango
echo "Install tango-idl"
docker exec cpp_tango make -C /home/tango/idl/build install
echo "Build cppTango:$CMAKE_BUILD_TYPE"
docker exec cpp_tango cmake -H/home/tango/src -B/home/tango/src/build -DCMAKE_VERBOSE_MAKEFILE=true -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE
docker exec cpp_tango make -C /home/tango/src/build -j 2
echo "Test log4tango"
docker exec cpp_tango /bin/sh -c 'cd /home/tango/src/build/log4tango; exec ctest -V'
echo "Test cppTango"
docker exec cpp_tango /bin/sh -c 'cd /home/tango/src/build/cpp_test_suite; exec ctest -V'