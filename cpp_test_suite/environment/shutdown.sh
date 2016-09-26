#!/usr/bin/env bash

echo "Shutdown test environment"

#kill tango-cs docker
docker stop tango_cs
docker rm tango_cs

#kill mysql-tango docker
docker stop mysql_db
docker rm mysql_db

echo "Revert tango_host file"
echo "TANGO_HOST=$TANGO_HOST"
echo "#!/bin/bash" > tango_host
echo "export TANGO_HOST=$TANGO_HOST" >> tango_host