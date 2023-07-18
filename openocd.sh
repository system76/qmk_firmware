#!/usr/bin/env bash

set -ex

cd openocd
sudo src/openocd \
    -f interface/cmsis-dap.cfg \
    -c "adapter speed 1000" \
    -f target/rp2040.cfg \
    -s tcl \
    "$@"
