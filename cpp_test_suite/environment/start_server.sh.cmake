#!/usr/bin/env bash
# vim: syntax=sh

set -e

instance="$1"
server="${2:-DevTest}"
server_path="${3:-cpp_test_ds}"

index="$(find \
    "$TANGO_TEST_CASE_DIRECTORY" \
    -maxdepth 1 \
    -mindepth 1 \
    -type f \
    -name "${server}_${instance}.*.out" | wc -l)"

echo "Starting $server/$instance"

"@PROJECT_BINARY_DIR@/${server_path}/${server}" "$instance" -v5 \
    &> "${TANGO_TEST_CASE_DIRECTORY}/${server}_${instance}.${index}.out" &

server_pid="$!"

mkdir -p "${TANGO_TEST_CASE_DIRECTORY}/server_pids/${server_pid}"

if hash tango_admin 2>/dev/null; then
    instance_lower="$(echo "$server/$instance" | awk '{print tolower($0)}')"
    tango_admin --ping-device "dserver/${instance_lower}" 7
else
    sleep 7
fi

echo "$server/$instance PID: ${server_pid}"
