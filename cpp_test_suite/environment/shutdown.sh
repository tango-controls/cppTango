#!/usr/bin/env bash

echo "Shutdown test environment"

docker-compose -f $1 stop