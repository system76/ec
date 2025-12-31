#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only

# SDCC 4.5.0 will incorrectly move the HOME segment, which contains the
# compiler-generated interrupt vector table, to code address 0x50 with ITE
# chips that declare the 16-byte signature at the default address of 0x40.
#
# Ref: https://github.com/system76/ec/issues/518

BUILD_DIR="${BUILD_DIR:-build}"
MAP_FILE="${BUILD_DIR}/ec.map"

if [ ! -d "${BUILD_DIR}" ]; then
    exit 0
fi

if [ ! -f "${MAP_FILE}" ]; then
    exit 0
fi

SEG_EXPECTED="00000000"
SEG_START=$(grep 's_HOME' "${MAP_FILE}" | awk '{ print $2 }')

if [ "${SEG_START}" != "${SEG_EXPECTED}" ]; then
    printf "\x1B[31mHOME segment starts at %s; image will not boot\x1B[0m\n" "${SEG_START}"
    exit 1
fi
