#!/bin/bash

echo "Setup test environment"

#run mysql-tango docker
docker run --name mysql_db \
    -e MYSQL_ROOT_PASSWORD=root \
    -e MYSQL_INITDB_SKIP_TZINFO=1 \
    -d \
    tangocs/mysql:9.2.2 \
    --sql-mode="" \
    --innodb=OFF \
    --default-storage-engine=MyISAM

#run tango-cs docker
CONTAINER=$(docker run --name tango_cs \
    -e TANGO_HOST=127.0.0.1:10000 \
    -e MYSQL_HOST=mysql_db:3306 \
    -e MYSQL_USER=tango \
    -e MYSQL_PASSWORD=tango \
    -e MYSQL_DATABASE=tango \
    --link mysql_db:mysql_db \
    -d tangocs/tango-cs:latest)

echo "CONTAINER=$CONTAINER"

IPADDR=$(docker inspect -f '{{ .NetworkSettings.IPAddress }}' $CONTAINER)
export TANGO_HOST=$IPADDR:10000
echo "TANGO_HOST=$TANGO_HOST"

echo "Create tango_host file"
echo "export TANGO_HOST=$TANGO_HOST" > tango_host

echo "Wait till tango-cs is online"

if hash tango_admin 2>/dev/null; then
  tango_admin --ping-database 30
else
  sleep 30
fi
