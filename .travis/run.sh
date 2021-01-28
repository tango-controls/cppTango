#!/usr/bin/env bash

set -e

# export all ENV variables from the docker container starting with TANGO_
eval $(docker inspect -f '{{range $index, $value := .Config.Env}}{{$value}} {{end}}' cpp_tango | \
  sed -e "s/ /\n/g" |                                                                            \
  grep "^TANGO_" |                                                                               \
  sed -e "s/^/export /")

echo "############################"
echo "CMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE"
echo "OS_TYPE=$OS_TYPE"
echo "TANGO_HOST=$TANGO_HOST"
echo "COVERALLS=$COVERALLS"
echo "USE_PCH=$USE_PCH"
echo "BUILD_SHARED_LIBS=$BUILD_SHARED_LIBS"
echo "TANGO_USE_USING_NAMESPACE=$TANGO_USE_USING_NAMESPACE"
echo "TANGO_USE_LIBCPP=$TANGO_USE_LIBCPP"
echo "TANGO_LIBCPP_INCLUDE=$TANGO_LIBCPP_INCLUDE"
echo "TANGO_LIBCPP_LIB=$TANGO_LIBCPP_LIB"
echo "WARNINGS_AS_ERRORS=$WARNINGS_AS_ERRORS"
echo "############################"

docker exec cpp_tango mkdir -p /home/tango/src/build

# set defaults
MAKEFLAGS=${MAKEFLAGS:- -j $(nproc)}
COVERALLS=${COVERALLS:-OFF}
TANGO_USE_LIBCPP=${TANGO_USE_LIBCPP:-OFF}
TANGO_LIBCPP_INCLUDE=${TANGO_LIBCPP_INCLUDE:-}
TANGO_LIBCPP_LIB=${TANGO_LIBCPP_LIB:-}
USE_PCH=${USE_PCH:-OFF}
BUILD_SHARED_LIBS=${BUILD_SHARED_LIBS:-ON}
WARNINGS_AS_ERRORS=${WARNINGS_AS_ERRORS:-OFF}
COVERALLS_MODULE_PATH=/home/tango/coveralls-cmake/cmake

docker exec cpp_tango cmake                                \
  -H/home/tango/src                                        \
  -B/home/tango/src/build                                  \
  -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}                 \
  -DCMAKE_VERBOSE_MAKEFILE=ON                              \
  -DCPPZMQ_BASE=/home/tango                                \
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}                   \
  -DUSE_PCH=${USE_PCH}                                     \
  -DTANGO_USE_USING_NAMESPACE=${TANGO_USE_USING_NAMESPACE} \
  -DTANGO_USE_LIBCPP=${TANGO_USE_LIBCPP}                   \
  -DTANGO_LIBCPP_INCLUDE=${TANGO_LIBCPP_INCLUDE}           \
  -DTANGO_LIBCPP_LIB=${TANGO_LIBCPP_LIB}                   \
  -DWARNINGS_AS_ERRORS=${WARNINGS_AS_ERRORS}               \
  -DTANGO_ENABLE_COVERAGE=${TANGO_ENABLE_COVERAGE:-OFF}    \
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
