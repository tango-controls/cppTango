#!/usr/bin/env bash

set -e

echo "############################"
echo "CMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE"
echo "OS_TYPE=$OS_TYPE"
echo "TANGO_HOST=$TANGO_HOST"
echo "COVERALLS=$COVERALLS"
echo "BUILD_SHARED_LIBS=$BUILD_SHARED_LIBS"
echo "############################"

docker exec cpp_tango mkdir -p /home/tango/src/build

# set defaults
MAKEFLAGS=${MAKEFLAGS:- -j 2}
COVERALLS=${COVERALLS:-OFF}
USE_PCH=${USE_PCH:-OFF}
BUILD_SHARED_LIBS=${BUILD_SHARED_LIBS:-ON}
COVERALLS_MODULE_PATH=/home/tango/coveralls-cmake/cmake

docker exec cpp_tango cmake                                \
  -H/home/tango/src                                        \
  -B/home/tango/src/build                                  \
  -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}                 \
  -DCMAKE_VERBOSE_MAKEFILE=ON                              \
  -DCPPZMQ_BASE=/home/tango                                \
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}                   \
  -DTANGO_ENABLE_COVERAGE=${TANGO_ENABLE_COVERAGE:-OFF}    \
  -DCOVERALLS=${COVERALLS}                                 \
  -DCOVERALLS_MODULE_PATH=${COVERALLS_MODULE_PATH}

if [[ "$COVERALLS" == "ON" ]]
then
  docker exec cpp_tango                                            \
    /home/tango/build-wrapper-linux-x86/build-wrapper-linux-x86-64 \
    --out-dir /home/tango/src/bw-output                            \
    make -C /home/tango/src/build
else
  docker exec cpp_tango make -C /home/tango/src/build
fi
