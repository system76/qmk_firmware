#!/usr/bin/env bash

set -ex

make system76/virgo_keyboard_test_1:default

FIRMWARE="$(realpath .build/system76_virgo_keyboard_test_1_default.elf)"
./openocd.sh -c "program ${FIRMWARE} verify reset exit"
