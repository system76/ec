#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only

readarray -t FILES < <(git ls-files '*.c' '*.h')

FMT_OPTS=(
    "-style=file"
    "--fallback-style=none"
    "--Werror"
)

if [[ "$1" = "apply" ]]; then
    clang-format "${FMT_OPTS[@]}" -i "${FILES[@]}"
else
    clang-format "${FMT_OPTS[@]}" --dry-run "${FILES[@]}"
fi
