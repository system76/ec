#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only

# Install dependencies for development.

# shellcheck disable=SC1091

set -eE

. /etc/os-release
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
    printf "\e[1m\e[31munsupported host:\e[0m %s\n" "${ID}"
    exit 1
fi

if [ -z "$CI" ]; then
    git submodule update --init --recursive
fi
