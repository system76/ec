#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: 2023 System76, Inc.

# Check that project is REUSE compliant.

# shellcheck disable=SC1091

LINT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
. "$LINT_DIR/util.sh"

echo -n "Checking for REUSE compliance..."

if ! command -v reuse > /dev/null; then
    skipped "reuse not found"
    exit 0
fi

if ! reuse lint --quiet; then
    failed

    reuse lint --lines
    exit 1
fi

passed
