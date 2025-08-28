#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: 2024 System76, Inc.

# Check if any C files or headers need to be formatted.

# shellcheck disable=SC1091

LINT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
. "$LINT_DIR/util.sh"

echo -n "Checking C style..."

if ! command -v uncrustify > /dev/null; then
    skipped "uncrustify not found"
    exit 0
fi

needs_formatting=()

for file in $(git ls-files '*.c' '*.h'); do
    if ! uncrustify -c .uncrustify.cfg -q --check "$file" >/dev/null 2>&1; then
        needs_formatting+=("$file")
    fi
done

if [[ "${#needs_formatting[@]}" != "0" ]]; then
    failed

    for file in "${needs_formatting[@]}"; do
        echo "- $file"
    done

    exit 1
fi

passed
