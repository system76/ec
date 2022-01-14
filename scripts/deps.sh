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
    sudo apt-get update
    sudo apt-get install \
        --no-install-recommends \
        --yes \
        avr-libc \
        avrdude \
        curl \
        gcc \
        gcc-avr \
        libc-dev \
        libhidapi-dev \
        libudev-dev \
        make \
        pkgconf \
        sdcc \
        xxd
elif [[ "${ID}" =~ "fedora" ]] || [[ "${ID_LIKE}" =~ "fedora" ]]; then
    sudo dnf install \
        --assumeyes \
        avr-gcc \
        avr-libc \
        avrdude \
        curl \
        gcc \
        make \
        sdcc \
        systemd-devel \
        vim-common
elif [[ "${ID}" =~ "arch" ]] || [[ "${ID_LIKE}" =~ "arch" ]]; then
    sudo pacman -S \
        --noconfirm \
        avr-gcc \
        avr-libc \
        avrdude \
        curl \
        gcc \
        make \
        pkgconf \
        sdcc \
        systemd-libs \
        vim
else
    msg "Please add support for your distribution to:"
    msg "scripts/deps.sh"
    exit 1
fi

msg "Initializing submodules"
git submodule update --init --recursive

RUSTUP_NEW_INSTALL=0
if which rustup &> /dev/null; then
    msg "Updating rustup"
    rustup self update
else
    RUSTUP_NEW_INSTALL=1
    msg "Installing Rust"
    curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs \
      | sh -s -- -y --default-toolchain none

    msg "Loading Rust environment"
    source "${HOME}/.cargo/env"
fi

msg "Installing pinned Rust toolchain and components"
rustup show

if [[ $RUSTUP_NEW_INSTALL = 1 ]]; then
    msg "rustup was just installed. Ensure cargo is on the PATH with:"
    echo -e "    source ~/.cargo/env\n"
fi

msg "\x1B[32mSuccessfully installed dependencies"
