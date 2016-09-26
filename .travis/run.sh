#!/usr/bin/env bash
echo "TANGO_HOST=$TANGO_HOST"

docker exec cpp_tango mkdir -p /src/idl/build
docker exec cpp_tango mkdir -p /src/build

docker exec cpp_tango cmake --build /src/idl/build -- /src/idl

echo "Install tango-idl"
docker exec cpp_tango make -C /src/idl/build install

echo "Build cppTango"
docker exec cpp_tango cmake --build /src/build -- /src
docker exec cpp_tango make -C /src/build
echo "Start test"
docker exec cpp_tango make -C /src/build test