#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: 2023 System76, Inc.

# Check if any shell scripts have issues.

# shellcheck disable=SC1091

LINT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
. "$LINT_DIR/util.sh"

echo -n "Checking shell scripts..."

if ! command -v shellcheck > /dev/null; then
    skipped "shellcheck not found"
    exit 0
fi

readarray -t FILES < <(git ls-files '*.sh')
needs_formatting=()

for file in "${FILES[@]}"; do
    # SC1091: Ignore external scripts
    if ! shellcheck -f quiet --exclude=SC1091 "$file"; then
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
