#!/usr/bin/env bash

build_dir="/home/tango/src/build"

if [[ "$COVERALLS" == "ON" ]]; then
  if ! docker exec \
      -w "${build_dir}" \
      cpp_tango \
      make coveralls
  then
    exit 1
  fi
else
  if ! docker exec \
      -w "${build_dir}" \
      cpp_tango \
      ../.travis/run_ctest.sh
  then
    exit 1
  fi
fi
