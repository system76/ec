#!/usr/bin/env bash

set -e
cargo build --release --manifest-path tool/Cargo.toml
sudo tool/target/release/system76_ectool "$@"
