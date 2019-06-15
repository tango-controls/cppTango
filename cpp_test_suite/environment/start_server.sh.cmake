#!/usr/bin/env bash
# vim: syntax=sh

instance="$1"

index="$(find \
  "$TANGO_TC_DIRECTORY" \
  -maxdepth 1 \
  -mindepth 1 \
  -type f \
  -name "DevTest_${instance}.*.out" | wc -l)"

echo "Starting DevTest/$instance"

"@PROJECT_BINARY_DIR@/cpp_test_ds/DevTest" "$instance" -v5 \
  &> "${TANGO_TC_DIRECTORY}/DevTest_${instance}.${index}.out" &

server_pid="$!"

mkdir -p "${TANGO_TC_DIRECTORY}/server_pids/${server_pid}"

if hash tango_admin 2>/dev/null; then
  instance_lower="$(echo "$instance" | awk '{print tolower($0)}')"
  tango_admin --ping-device "dserver/devtest/${instance_lower}" 7
else
  sleep 7
fi

echo "Done. PID = ${server_pid}"
