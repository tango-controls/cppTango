#!/bin/bash

echo "Setup test environment"

docker-compose -f $1 up start_dependencies

docker-compose -f $1 up -d tango-test

#export TANGO_HOST
TANGO_HOST=localhost:10000
echo "TANGO_HOST=$TANGO_HOST"
export TANGO_HOST