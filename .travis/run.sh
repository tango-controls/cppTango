#!/usr/bin/env bash
docker exec cpp_tango mkdir -p /src/idl/build
docker exec cpp_tango mkdir -p /src/build

docker exec cpp_tango cmake -H/src/idl -B/src/idl/build

echo "Install tango-idl"
docker exec cpp_tango make -C /src/idl/build install

echo "Build cppTango"
docker exec cpp_tango cmake -H/src -B/src/build -DCMAKE_VERBOSE_MAKEFILE=true --build /src/build -- test