#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only

set -eE
EXCLUDED_BOARDS=(
    "system76/common"
    "arduino/uno"
    "arduino/micro"
)

BOARDS=$(find src/board -type d -maxdepth 2 -mindepth 2 2>/dev/null \
  | grep -vFf <(printf '%s\n' "${EXCLUDED_BOARDS[@]}") \
  | jq -sRc 'split("\n")[:-1] | map(split("/")) | map({vendor: .[2], board: .[3]})'
)

echo "boards=${BOARDS}"