#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: 2023 System76, Inc.

make lint 2>/dev/null || {
    echo -e "\nissues found, not committing"
    exit 1
}
