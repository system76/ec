#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only

# Check if any C files or headers need to be formatted.

LINT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
. "$LINT_DIR/util.sh"

echo -n "Checking C style..."

if ! command -v clang-format > /dev/null; then
    skipped "clang-format not found"
    exit 0
fi

readarray -t FILES < <(git ls-files '*.c' '*.h')

FMT_OPTS=(
    "-style=file"
    "--fallback-style=none"
    "--dry-run"
    "--Werror"
)

# NOTE: It is too slow to run clang-format on every file individually to report
# which ones fail. Leave it up to the user to apply formatting via `make fmt`.

_output=$(clang-format "${FMT_OPTS[@]}" "${FILES[@]}" 2>&1)
if [[ $_output != "" ]]; then
    failed
    echo "$_output"
    exit 1
fi

passed
