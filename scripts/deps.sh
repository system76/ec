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
    sudo apt-get install --no-install-recommends --yes \
        avr-libc \
        avrdude \
        gcc \
        gcc-avr \
        libc-dev \
        libhidapi-dev \
        libudev-dev \
        make \
        pkgconf \
        rustup \
        sdcc \
        shellcheck \
        uncrustify \
        xxd
elif [[ "${ID}" =~ "fedora" ]] || [[ "${ID_LIKE}" =~ "fedora" ]]; then
    sudo dnf install --assumeyes \
        avr-gcc \
        avr-libc \
        avrdude \
        gcc \
        make \
        rustup \
        sdcc \
        ShellCheck \
        systemd-devel \
        uncrustify \
        vim-common
elif [[ "${ID}" =~ "arch" ]] || [[ "${ID_LIKE}" =~ "arch" ]]; then
    sudo pacman -S --noconfirm \
        avr-gcc \
        avr-libc \
        avrdude \
        gcc \
        make \
        pkgconf \
        rustup \
        sdcc \
        shellcheck \
        systemd-libs \
        uncrustify \
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

RUSTUP_NEW_INSTALL=0
if ! command -v rustup >/dev/null 2>&1; then
    # Fedora "rustup" package really just installs "rustup-init"
    if command -v rustup-init >/dev/null 2>&1; then
        RUSTUP_NEW_INSTALL=1
        msg "Installing Rust"
        rustup-init -y \
            --default-toolchain none \
            --profile minimal \
            --no-update-default-toolchain
        . "${HOME}/.cargo/env"
    fi
fi

msg "Installing pinned Rust toolchain and components"
# Ref: https://github.com/rust-lang/rustup/issues/3635
rustup show active-toolchain || rustup toolchain install

if [[ $RUSTUP_NEW_INSTALL = 1 ]]; then
    msg "rustup was just installed. Ensure cargo is on the PATH with:"
    echo -e "    source ~/.cargo/env\n"
fi

msg "\x1B[32mSuccessfully installed dependencies"
