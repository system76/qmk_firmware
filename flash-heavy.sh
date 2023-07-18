#!/usr/bin/env bash

set -ex

make system76/launch_heavy_3:default

FIRMWARE="$(realpath .build/system76_launch_heavy_3_default.elf)"
./openocd.sh -c "program ${FIRMWARE} verify reset exit"
