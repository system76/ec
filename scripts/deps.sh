#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: 2021 System76, Inc.

# shellcheck disable=SC1091

set -eE

. /etc/os-release
if [ "${ID}" = "debian" ] || [[ "${ID_LIKE}" =~ "debian" ]]; then
    sudo apt-get update
    sudo apt-get install --no-install-recommends --yes \
        avr-libc \
        avrdude \
        ca-certificates \
        curl \
        gcc \
        gcc-avr \
        libc-dev \
        libhidapi-dev \
        libudev-dev \
        make \
        pkgconf \
        reuse \
        sdcc \
        shellcheck \
        uncrustify \
        xxd
elif [ "${ID}" = "fedora" ] || [[ "${ID_LIKE}" =~ "fedora" ]]; then
    sudo dnf install --assumeyes \
        avr-gcc \
        avr-libc \
        avrdude \
        gcc \
        make \
        reuse \
        rustup \
        sdcc \
        ShellCheck \
        systemd-devel \
        uncrustify \
        vim-common
elif [ "${ID}" = "arch" ] || [[ "${ID_LIKE}" =~ "arch" ]]; then
    sudo pacman -S --noconfirm \
        avr-gcc \
        avr-libc \
        avrdude \
        gcc \
        make \
        pkgconf \
        reuse \
        rustup \
        sdcc \
        shellcheck \
        systemd-libs \
        uncrustify \
        vim
else
    printf "\x1B[1m\x1B[31munsupported host:\x1B[0m %s\n" "${ID}"
    exit 1
fi

if ! command -v rustup >/dev/null 2>&1; then
    if command -v rustup-init >/dev/null 2>&1; then
        rustup-init -y \
            --default-toolchain stable \
            --profile minimal \
            --no-update-default-toolchain
    else
        curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs \
            | sh -s -- -y --default-toolchain stable
    fi

    . "${HOME}/.cargo/env"
fi

rustup show active-toolchain || rustup toolchain install
