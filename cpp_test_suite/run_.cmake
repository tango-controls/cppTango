#!/bin/bash
source @CMAKE_CURRENT_BINARY_DIR@/../environment/tango_host
echo "TANGO_HOST=$TANGO_HOST"
@CMAKE_CURRENT_BINARY_DIR@/@EXECUTABLE@ $1 $2 $3 $4 $5 $6