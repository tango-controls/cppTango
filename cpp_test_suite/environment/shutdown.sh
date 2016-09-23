#!/usr/bin/env bash

echo "Shutdown test environment"

#kill tango-cs docker
docker stop tango_cs
docker rm tango_cs

#kill mysql-tango docker
docker stop mysql_db
docker rm mysql_db

