#!/usr/bin/env bash
# vim: syntax=sh

set -e

if [ $# -lt 1 ]; then
    echo "usage: $0 <executable> [args...]"
    exit 1
fi

if ! hash docker 2>/dev/null; then
  echo "Can not run the tests as docker is missing"
  exit 1
fi

tc_program="$1"
tc_run_name="$(basename "$tc_program")_$(date '+%Y%m%d.%H%M%S.%N')"
shift 1

tc_mysql_container="mysql_db_${tc_run_name}"
tc_tango_container="tango_cs_${tc_run_name}"

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
    eval $(
        set -e
        source "@CMAKE_CURRENT_BINARY_DIR@/setup_database.sh" \
            "$tc_mysql_container" \
            "$tc_tango_container"
        echo "export TANGO_HOST=$TANGO_HOST"
    )
    "@CMAKE_CURRENT_BINARY_DIR@/setup_devices.sh"
fi

set +e
set -o pipefail

"$tc_program" "$@" 2>&1 | tee "${TANGO_TEST_CASE_DIRECTORY}/testcase.out"
tc_exit_code="$?"

set +o pipefail
set -e

echo "$tc_exit_code" >  "${TANGO_TEST_CASE_DIRECTORY}/testcase_exit_code.out"
exit "$tc_exit_code"
