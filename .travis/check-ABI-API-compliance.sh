#!/usr/bin/env bash

set -e

if [[ -z "$CI_TARGET_BRANCH" ]]
then
  CI_TARGET_BRANCH=tango-9-lts
fi

if [[ -z "$CMAKE_BUILD_PARALLEL_LEVEL" ]]
then
  export CMAKE_BUILD_PARALLEL_LEVEL=$(grep -c ^processor /proc/cpuinfo)
fi

function print_abi_api_breakages() {
   echo "ABI breakages detected:"
   cat compat_reports/libtango/${old_revision}_to_${new_revision}/abi_affected.txt | c++filt
   echo "API breakages detected:"
   cat compat_reports/libtango/${old_revision}_to_${new_revision}/src_affected.txt | c++filt
   exit 1
}

# $1 string prefix
# $2 git revision
function generate_info() {
  local prefix=$1
  local revision=$2

  git worktree remove $prefix-branch || true
  git -c advice.detachedHead=false worktree add ${prefix}-branch ${revision}
  mkdir ${prefix}-branch/build
  cd ${prefix}-branch/build
  cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=OFF -DUSE_PCH=ON ..
  cmake --build .
  abi-dumper libtango.so -o ${base}/libtango-${prefix}-${revision}.dump -lver ${revision}
  cd "${base}"
}

new_revision=$(git rev-parse HEAD)

# old_revision is the first parent commit not in the history of this branch
# see the help of git merge-base for the gritty detailed explanation
old_revision=$(git merge-base --fork-point origin/${CI_TARGET_BRANCH} HEAD)

if [[ "${new_revision}" == "${old_revision}" ]]
then
  echo "Nothing to do"
  exit 0
fi

base=$(pwd)

generate_info "old" ${old_revision}
generate_info "new" ${new_revision}

# Compare results
abi-compliance-checker -l libtango -old ${base}/libtango-old-${old_revision}.dump \
                                   -new ${base}/libtango-new-${new_revision}.dump \
                                   -list-affected || print_abi_api_breakages
