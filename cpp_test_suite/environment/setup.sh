#!/bin/bash

echo "Setup test environment"

function run_mysql_container {
    docker run \
        --rm \
        --name $1 \
        -e MYSQL_ROOT_PASSWORD=root \
        -e MYSQL_INITDB_SKIP_TZINFO=1 \
        -d \
        tangocs/mysql:9.2.2 \
        --sql-mode="" \
        --innodb=OFF \
        --default-storage-engine=MyISAM
}

function run_tango_container {
    docker run \
        --rm \
        --name $1 \
        -e TANGO_HOST=127.0.0.1:10000 \
        -e MYSQL_HOST=$2:3306 \
        -e MYSQL_USER=tango \
        -e MYSQL_PASSWORD=tango \
        -e MYSQL_DATABASE=tango \
        --link $2:$2 \
        -d \
        tangocs/tango-cs:latest
}

run_mysql_container mysql_db
run_tango_container tango_cs mysql_db

IPADDR=$(docker inspect -f '{{ .NetworkSettings.IPAddress }}' tango_cs)

export TANGO_HOST=$IPADDR:10000

echo "TANGO_HOST=$TANGO_HOST"

echo "Create tango_host file"

cat << EOF > tango_host
#!/bin/bash
export TANGO_HOST=$TANGO_HOST
EOF

echo "Wait till tango-cs is online"
if hash tango_admin 2>/dev/null; then
    tango_admin --ping-database 30
else
    sleep 30
fi
