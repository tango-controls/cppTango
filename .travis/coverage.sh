#!/usr/bin/env bash

docker exec cpp_tango /bin/sh -c 'cd /home/tango/src; gcovr --root /home/tango/src --use-gcov-files --xml --output report.xml --verbose /home/tango/src/build'