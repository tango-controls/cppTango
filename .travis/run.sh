#!/usr/bin/env bash

set -e

echo "############################"
echo "CMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE"
echo "OS_TYPE=$OS_TYPE"
echo "TANGO_HOST=$TANGO_HOST"
echo "COVERALLS=$COVERALLS"
echo "USE_PCH=$USE_PCH"
echo "TANGO_USE_USING_NAMESPACE=$TANGO_USE_USING_NAMESPACE"
echo "WARNINGS_AS_ERRORS=$WARNINGS_AS_ERRORS"
echo "############################"

docker exec cpp_tango mkdir -p /home/tango/src/build

# set defaults
MAKEFLAGS=${MAKEFLAGS:- -j $(nproc)}
COVERALLS=${COVERALLS:-OFF}
USE_PCH=${USE_PCH:-OFF}
WARNINGS_AS_ERRORS=${WARNINGS_AS_ERRORS:-OFF}
COVERALLS_MODULE_PATH=/home/tango/coveralls-cmake/cmake

docker exec cpp_tango cmake                                \
  -H/home/tango/src                                        \
  -B/home/tango/src/build                                  \
  -DCMAKE_VERBOSE_MAKEFILE=ON                              \
  -DCPPZMQ_BASE=/home/tango                                \
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}                   \
  -DUSE_PCH=${USE_PCH}                                     \
  -DTANGO_USE_USING_NAMESPACE=${TANGO_USE_USING_NAMESPACE} \
  -DWARNINGS_AS_ERRORS=${WARNINGS_AS_ERRORS}               \
  -DCOVERALLS=${COVERALLS}                                 \
  -DCOVERALLS_MODULE_PATH=${COVERALLS_MODULE_PATH}

if [[ "$SONAR_SCANNER" == "ON" ||  "$COVERALLS" == "ON" ]]
then
  docker exec cpp_tango                                            \
    /home/tango/build-wrapper-linux-x86/build-wrapper-linux-x86-64 \
    --out-dir /home/tango/src/bw-output                            \
    make -C /home/tango/src/build $MAKEFLAGS
else
  docker exec cpp_tango make -C /home/tango/src/build $MAKEFLAGS
fi
