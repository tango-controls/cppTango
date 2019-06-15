#!/usr/bin/env bash
# vim: syntax=sh

pids="$(find \
    "$TANGO_TEST_CASE_DIRECTORY/server_pids/" \
    -maxdepth 1 \
    -mindepth 1 \
    -type d \
    -printf "%f\n" \
    2> /dev/null)"

echo "Killing PIDS: " $pids

kill $pids &>/dev/null

sleep 2
