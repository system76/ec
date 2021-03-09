#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
# Check files do not use tabs for indentation

git ls-files '*.[c\|h\|rs\|sh]' | xargs grep --line-number $'^\s*\t' && exit 1
exit 0
