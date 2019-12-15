#!/usr/bin/env bash

if ! ctest \
    --output-on-failure \
    --parallel 8
then
    if ! ctest \
        --output-on-failure \
        --rerun-failed \
        --repeat-until-fail 2
    then
        exit 1
    fi
fi
