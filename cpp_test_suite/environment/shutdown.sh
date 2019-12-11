#!/usr/bin/env bash

echo "Shutdown test environment"
docker stop tango_cs
docker stop mysql_db

echo "Revert tango_host file"
echo "TANGO_HOST=$TANGO_HOST"
echo "#!/bin/bash" > tango_host
echo "export TANGO_HOST=$TANGO_HOST" >> tango_host
