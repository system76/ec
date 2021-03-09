#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
# Check that all files have a SPDX license identifier

set -e

# File patterns to check
FILE_TYPES=(
    '*.c'
    '*.h'
    '*.mk'
    '*.rs'
    '*.sh'
    'Makefile'
)

ret=0

for ft in "${FILE_TYPES[@]}"; do
    files=$(git ls-files "$ft")
    for f in ${files}; do
        # Skip empty files
        if [[ "$(wc -l < "$f")" = "0" ]]; then
            continue
        fi

        # SPDX must appear at head of file
        if ! head "$f" | grep -q 'SPDX-License-Identifier:'; then
            echo "$f: Missing SPDX identifier"
            ret=1
        fi
    done
done

exit ${ret}
