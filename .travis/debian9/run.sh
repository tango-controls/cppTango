#!/usr/bin/env bash
docker exec cpp_tango mkdir -p /home/tango/src/build

echo "Build cppTango:$CMAKE_BUILD_TYPE"
docker exec cpp_tango cmake \
  -H/home/tango/src \
  -B/home/tango/src/build \
  -DCMAKE_VERBOSE_MAKEFILE=true \
  -DCPPZMQ_BASE=/home/tango \
  -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE \
  -DUSE_PCH=${USE_PCH:-OFF}
if [ $? -ne "0" ]
then
    exit -1
fi

docker exec cpp_tango make -C /home/tango/src/build -j 2
