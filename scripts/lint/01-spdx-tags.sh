#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only

# Check that all files have a SPDX license identifier
# TODO: Validate license tags against a whitelist

LINT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
. "$LINT_DIR/util.sh"

echo -n "Checking for SPDX tags..."

EXCLUDES=(
    # Ignore license files
    ':!:LICENSE'
    ':!:**/LICENSE'
    # Ignore cargo files
    ':!:**/Cargo.lock'
    ':!:**/Cargo.toml'
    ':!:rust-toolchain.toml'
    # Ignore text files
    ':!:*.md'
    ':!:*.txt'
    # Ignore dotfiles
    ':!:\.*'
    ':!:**/\.*'
)

needs_tag=()

for file in $(git ls-files "${EXCLUDES[@]}"); do
    # Only check regular files
    if [[ ! -f "$file" ]]; then
        continue
    fi

    # SPDX must appear at head of file
    if ! head "$file" | grep -q 'SPDX-License-Identifier:'; then
        needs_tag+=("$file")
    fi
done

if [[ "${#needs_tag[@]}" != "0" ]]; then
    failed

    for file in "${needs_tag[@]}"; do
        echo "- $file"
    done

    exit 1
fi

passed
