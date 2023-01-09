#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only

# Run all lints.

LINT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
LINTS=$(find "$LINT_DIR" -type f -name "[0-9][0-9]-*" | sort)
FAILED=0

for lint in $LINTS; do
    $lint || FAILED=1
done

[[ "$FAILED" = "0" ]] || exit 1
