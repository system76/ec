#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only

set -eE

function msg {
  echo -e "\x1B[1m$*\x1B[0m" >&2
}

trap 'msg "\x1B[31mFailed to install dependencies!"' ERR

source /etc/os-release

msg "Installing system build dependencies"
if [[ "${ID}" =~ "debian" ]] || [[ "${ID_LIKE}" =~ "debian" ]]; then
    sudo apt-get install \
        --no-install-recommends \
        --yes \
        avr-libc \
        avrdude \
        curl \
        gcc-avr \
        libudev-dev \
        make \
        sdcc
elif [[ "${ID}" =~ "fedora" ]] || [[ "${ID_LIKE}" =~ "fedora" ]]; then
    sudo dnf install \
        --assumeyes \
        avr-gcc \
        avr-libc \
        avrdude \
        curl \
        make \
        sdcc \
        systemd-devel
else
    msg "Please add support for your distribution to:"
    msg "scripts/deps.sh"
    exit 1
fi

msg "Initializing submodules"
git submodule update --init --recursive

if ! which rustup &> /dev/null; then
    msg "Installing Rust"
    curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs \
      | sh -s -- -y --default-toolchain none

    msg "Loading Rust environment"
    source "${HOME}/.cargo/env"
fi

msg "Installing pinned Rust toolchain and components"
rustup show

msg "\x1B[32mSuccessfully installed dependencies"
