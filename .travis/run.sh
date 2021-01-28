#!/usr/bin/env bash

set -e

echo "############################"
echo "CMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE"
echo "OS_TYPE=$OS_TYPE"
echo "TANGO_HOST=$TANGO_HOST"
echo "COVERALLS=$COVERALLS"
echo "USE_PCH=$USE_PCH"
echo "BUILD_SHARED_LIBS=$BUILD_SHARED_LIBS"
echo "TANGO_USE_USING_NAMESPACE=$TANGO_USE_USING_NAMESPACE"
echo "WARNINGS_AS_ERRORS=$WARNINGS_AS_ERRORS"
echo "TOOLCHAIN_FILE=$TOOLCHAIN_FILE"
echo "############################"

docker exec cpp_tango mkdir -p /home/tango/src/build

# set defaults
MAKEFLAGS=${MAKEFLAGS:- -j $(nproc)}
COVERALLS=${COVERALLS:-OFF}
USE_PCH=${USE_PCH:-OFF}
BUILD_SHARED_LIBS=${BUILD_SHARED_LIBS:-ON}
WARNINGS_AS_ERRORS=${WARNINGS_AS_ERRORS:-OFF}
COVERALLS_MODULE_PATH=/home/tango/coveralls-cmake/cmake
SOURCE_DIR=/home/tango/src
BUILD_DIR=${SOURCE_DIR}/build

ADDITIONAL_ARGS=""

if [[ "$SONAR_SCANNER" == "ON" ||  "$COVERALLS" == "ON" ]]
then
  ADDITIONAL_ARGS="${ADDITIONAL_ARGS} -DCOVERALLS=${COVERALLS} -DCOVERALLS_MODULE_PATH=${COVERALLS_MODULE_PATH}"
fi

if [[ -f "$TOOLCHAIN_FILE" && -s "$TOOLCHAIN_FILE" ]]
then
  ADDITIONAL_ARGS="${ADDITIONAL_ARGS} -DCMAKE_TOOLCHAIN_FILE=${SOURCE_DIR}/${TOOLCHAIN_FILE}"
fi

docker exec cpp_tango cmake                                \
  -H${SOURCE_DIR}                                          \
  -B${BUILD_DIR}                                           \
  -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}                 \
  -DCMAKE_VERBOSE_MAKEFILE=ON                              \
  -DCPPZMQ_BASE=/home/tango                                \
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}                   \
  -DUSE_PCH=${USE_PCH}                                     \
  -DTANGO_USE_USING_NAMESPACE=${TANGO_USE_USING_NAMESPACE} \
  -DWARNINGS_AS_ERRORS=${WARNINGS_AS_ERRORS}               \
  -DTANGO_ENABLE_COVERAGE=${TANGO_ENABLE_COVERAGE:-OFF}    \
  ${ADDITIONAL_ARGS}

if [[ "$SONAR_SCANNER" == "ON" ||  "$COVERALLS" == "ON" ]]
then
  docker exec cpp_tango                                            \
    /home/tango/build-wrapper-linux-x86/build-wrapper-linux-x86-64 \
    --out-dir ${SOURCE_DIR}/bw-output                              \
    make -C ${BUILD_DIR} $MAKEFLAGS
else
  docker exec cpp_tango make -C ${BUILD_DIR} $MAKEFLAGS
fi
