#!/usr/bin/env bash

set -e

echo "Install tango"
docker exec --user root cpp_tango make -C /home/tango/src/build install
