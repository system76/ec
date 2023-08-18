#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only

# Install dependencies for development.

# shellcheck disable=SC1091

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
        clang-format \
        curl \
        gcc \
        gcc-avr \
        libc-dev \
        libhidapi-dev \
        libudev-dev \
        make \
        pkgconf \
        sdcc \
        shellcheck \
        xxd
elif [[ "${ID}" =~ "fedora" ]] || [[ "${ID_LIKE}" =~ "fedora" ]]; then
    sudo dnf install \
        --assumeyes \
        avr-gcc \
        avr-libc \
        avrdude \
        clang-tools-extra \
        curl \
        gcc \
        make \
        sdcc \
        ShellCheck \
        systemd-devel \
        vim-common
elif [[ "${ID}" =~ "arch" ]] || [[ "${ID_LIKE}" =~ "arch" ]]; then
    sudo pacman -S \
        --noconfirm \
        avr-gcc \
        avr-libc \
        avrdude \
        clang \
        curl \
        gcc \
        make \
        pkgconf \
        sdcc \
        shellcheck \
        systemd-libs \
        vim
else
    msg "Please add support for your distribution to:"
    msg "scripts/deps.sh"
    exit 1
fi

msg "Initializing submodules"
git submodule update --init --recursive

msg "Installing git hooks"
make git-config

./scripts/install-rust.sh
