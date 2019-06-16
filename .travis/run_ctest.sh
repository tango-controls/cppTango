#!/usr/bin/env bash

build_dir="/home/tango/src/build"

if ! docker exec \
    -w "${build_dir}" \
    cpp_tango \
    ctest \
    -output-on-failure \
    -j8
then
  if ! docker exec \
      -w "${build_dir}" \
      cpp_tango ctest \
      --output-on-failure \
      --rerun-failed \
      --repeat-until-fail 2
  then
    exit 1
  fi
fi
