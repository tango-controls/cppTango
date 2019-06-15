#!/usr/bin/env bash
# vim: syntax=sh

if [ $# -lt 1 ]; then
  echo "usage: $0 <executable> [args...]"
  exit 1
fi

tc_exec="$1"
tc_exec_name="$(basename "$tc_exec")"
tc_name="${tc_exec_name}_$(date '+%Y%m%d.%H%M%S.%N')"
shift 1

tc_tango_container="${tc_name}_tango_cs"
tc_mysql_container="${tc_name}_mysql_db"

tc_directory="@PROJECT_BINARY_DIR@/test_results/${tc_name}"
mkdir -p "${tc_directory}"

function teardown {
  kill "$tc_fwdtest_pid" "$tc_devtest_pid" &> /dev/null
  docker stop "$tc_tango_container" "$tc_mysql_container" &> /dev/null
}


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
    teardown
    exit 1
  fi
else
  sleep 30
fi


# -- setup devices ------------------------------------------------------------

instance_lower="$(echo '@INST_NAME@' | awk '{print tolower($0)}')"

"@PROJECT_BINARY_DIR@/new_tests/conf_devtest" \
  @SERV_NAME@/@INST_NAME@ \
  @DEV1@ \
  @DEV2@ \
  @DEV3@ \
  @DEV1_ALIAS@ \
  @ATTR_ALIAS@ \
  @FWD_DEV@ \
  @DEV20@ \
  &> "${tc_directory}/conf_devtest.out"

"@PROJECT_BINARY_DIR@/cpp_test_ds/DevTest" @INST_NAME@ -v5 \
  &> "${tc_directory}/DevTest_@INST_NAME@.out" &

tc_devtest_pid="$!"

if hash tango_admin 2>/dev/null; then
  if ! tango_admin --ping-device "dserver/devtest/${instance_lower}" 3; then
    echo "Failed to ping DevTest"
    teardown
    exit 1
  fi
else
  sleep 3
fi

"@PROJECT_BINARY_DIR@/cpp_test_ds/fwd_ds/FwdTest" @INST_NAME@ -v5 \
  &> "${tc_directory}/FwdTest_@INST_NAME@.out" &

tc_fwdtest_pid="$!"

if hash tango_admin 2>/dev/null; then
  if ! tango_admin --ping-device "dserver/fwdtest/${instance_lower}" 3; then
    echo "Failed to ping FwdTest"
    teardown
    exit 1
  fi
else
  sleep 3
fi


# --- run test ----------------------------------------------------------------

export TANGO_TC_DIRECTORY="$tc_directory"

set -o pipefail

"$tc_exec" "$@" 2>&1 | tee "${tc_directory}/${tc_exec_name}.out"

tc_exit_code="$?"

set +o pipefail

echo "$tc_exit_code" >  "${tc_directory}/${tc_exec_name}_exit_code.out"


# --- teardown ----------------------------------------------------------------

(
  extra_pids="$(find \
    "${tc_directory}/server_pids/" \
    -maxdepth 1 \
    -mindepth 1 \
    -type d \
    -printf "%f\n" \
    2> /dev/null)"

  kill $extra_pids &> /dev/null

  teardown
) &


# --- exit --------------------------------------------------------------------

exit "$tc_exit_code"
