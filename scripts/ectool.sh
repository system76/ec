#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only

set -e
cargo build --release --manifest-path tool/Cargo.toml
sudo tool/target/release/dasharo_ectool "$@"
