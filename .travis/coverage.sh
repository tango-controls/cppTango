#!/usr/bin/env bash

docker exec cpp_tango /bin/sh -c 'cd /home/tango/src; gcovr --root ./cppapi --use-gcov-files --object-directory=./build --xml --output report.xml --verbose'