#!/usr/bin/env bash
docker exec cpp_tango mkdir -p /src/idl/build
docker exec cpp_tango mkdir -p /src/build

echo "Build tango-idl"
docker exec cpp_tango cmake -H/src/idl -B/src/idl/build
if [ $? -ne 0 ]; then
    exit $?
fi
echo "Install tango-idl"
docker exec cpp_tango make -C /src/idl/build install
if [ $? -ne 0 ]; then
    exit $?
fi
echo "Build cppTango"
docker exec cpp_tango cmake -H/src -B/src/build -DCMAKE_VERBOSE_MAKEFILE=true
if [ $? -ne 0 ]; then
    exit $?
fi
docker exec cpp_tango make -C /src/build -j 2
if [ $? -ne 0 ]; then
    exit $?
fi
echo "Test cppTango"
docker exec cpp_tango /bin/sh -c 'cd /src/build; exec ctest -V'
if [ $? -ne 0 ]; then
    exit $?
fi