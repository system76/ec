#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: 2023 System76, Inc.

passed() {
    echo -e "\x1B[32mPASSED\x1B[0m"
}

skipped() {
    local reason=$1
    echo -e "\x1B[33mSKIPPED\x1B[0m ($reason)"
}

failed() {
    echo -e "\x1B[31mFAILED\x1B[0m"
}
