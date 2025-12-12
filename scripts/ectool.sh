#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: 2020 System76, Inc.

set -e
cargo build --release --quiet --manifest-path tools/system76_ectool/Cargo.toml
sudo tools/system76_ectool/target/release/system76_ectool "$@"
