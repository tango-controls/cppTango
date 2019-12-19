#!/usr/bin/env bash

if ! ctest \
    --output-on-failure \
    --parallel $(nproc)
then
    if ! ctest \
        --output-on-failure \
        --rerun-failed \
        --repeat-until-fail 2
    then
        exit 1
    fi
fi
