#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only

cppcheck --quiet --force \
    --enable=all \
    --std=c11 \
    --suppressions-list=.cppcheck-suppressions.txt \
    --error-exitcode=1 \
    src/
