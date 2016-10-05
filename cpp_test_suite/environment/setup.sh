#!/bin/bash

echo "Setup test environment"

#run mysql-tango docker
docker run --name mysql_db \
    -e MYSQL_ROOT_PASSWORD=root \
    -d tangocs/mysql:9.2.2 --sql-mode=""

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
#export TANGO_HOST
TANGO_HOST=$IPADDR:10000
echo "TANGO_HOST=$TANGO_HOST"
export TANGO_HOST

echo "Create tango_host file"
echo "#!/bin/bash" > tango_host
echo "export TANGO_HOST=$TANGO_HOST" >> tango_host

echo "Wait till tango-cs is online"
#TODO notification?
sleep 30

#ctest