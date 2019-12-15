#!/usr/bin/env bash

if [ $RUN_TESTS = "OFF" ]
then
  echo "Skipping tests as requested"
  exit 0
fi

build_dir="/home/tango/src/build"

set -e

if [[ "$COVERALLS" == "ON" ]]; then
    docker exec -w "${build_dir}" cpp_tango make coveralls
else
    docker exec -w "${build_dir}" cpp_tango ../.travis/run_ctest.sh
fi
