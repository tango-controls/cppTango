#!/usr/bin/env bash
# vim: syntax=sh

set -e

if [ $# -lt 1 ]; then
    echo "usage: $0 <executable> [args...]"
    exit 1
fi

tc_program="$1"
tc_run_name="$(basename "$tc_program")_$(date '+%Y%m%d.%H%M%S.%N')"
shift 1

tc_tango_container="tango_cs_${tc_run_name}"
tc_mysql_container="mysql_db_${tc_run_name}"

export TANGO_TEST_CASE_DIRECTORY="@PROJECT_BINARY_DIR@/test_results/${tc_run_name}"
mkdir -p "$TANGO_TEST_CASE_DIRECTORY"

function teardown {
    (
        # close stdout and stderr to allow ctest finish the test early
        exec 1<&-
        exec 2<&-
        "@CMAKE_CURRENT_BINARY_DIR@/kill_server.sh" &>/dev/null || true
        docker stop "$tc_tango_container" "$tc_mysql_container" &>/dev/null || true
    ) &
}


if [[ -z "$TANGO_TEST_CASE_SKIP_FIXTURE" ]]; then

    trap teardown EXIT

# --- setup database ----------------------------------------------------------

    docker run \
        --name "$tc_mysql_container" \
        --rm \
        -e MYSQL_ROOT_PASSWORD=root \
        -e MYSQL_INITDB_SKIP_TZINFO=1 \
        -d \
        tangocs/mysql:9.2.2 \
        --sql-mode="" \
        --innodb=OFF \
        --default-storage-engine=MyISAM \
        > /dev/null

    docker run \
        --name "$tc_tango_container" \
        --rm \
        -e TANGO_HOST=127.0.0.1:10000 \
        -e MYSQL_HOST=mysql_db:3306 \
        -e MYSQL_USER=tango \
        -e MYSQL_PASSWORD=tango \
        -e MYSQL_DATABASE=tango \
        --link "$tc_mysql_container":mysql_db \
        -d \
        tangocs/tango-cs:latest \
        > /dev/null

    tc_tango_ipaddr="$(docker inspect \
        -f '{{ .NetworkSettings.IPAddress }}' \
        "$tc_tango_container")"

    export TANGO_HOST="$tc_tango_ipaddr:10000"

    if hash tango_admin 2>/dev/null; then
        if ! tango_admin --ping-database 30; then
            echo "Failed to ping database"
            exit 1
        fi
    else
        sleep 30
    fi

# -- setup devices ------------------------------------------------------------

    "@PROJECT_BINARY_DIR@/new_tests/conf_devtest" \
        @SERV_NAME@/@INST_NAME@ \
        @DEV1@ \
        @DEV2@ \
        @DEV3@ \
        @DEV1_ALIAS@ \
        @ATTR_ALIAS@ \
        @FWD_DEV@ \
        @DEV20@ \
        &> "${TANGO_TEST_CASE_DIRECTORY}/conf_devtest.out"

    "@CMAKE_CURRENT_BINARY_DIR@/start_server.sh" @INST_NAME@ DevTest cpp_test_ds

    "@CMAKE_CURRENT_BINARY_DIR@/start_server.sh" @INST_NAME@ FwdTest cpp_test_ds/fwd_ds

fi  # TANGO_TEST_CASE_SKIP_FIXTURE


# --- run test ----------------------------------------------------------------

set +e
set -o pipefail

"$tc_program" "$@" 2>&1 | tee "${TANGO_TEST_CASE_DIRECTORY}/testcase.out"

tc_exit_code="$?"

set +o pipefail
set -e

echo "$tc_exit_code" >  "${TANGO_TEST_CASE_DIRECTORY}/testcase_exit_code.out"

exit "$tc_exit_code"
