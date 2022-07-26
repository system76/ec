#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only

# Generate LVFS MetaInfo from a template file.

warn() {
    echo -e "\x1B[33m$*\x1B[0m" >&2
}

error() {
    echo -e "\x1B[31m$*\x1B[0m" >&2
}

TEMPLATE=$1
if [[ -z "$TEMPLATE" ]]; then
    echo "$0: <template>" >&2
    exit 1
fi

export LVFS_VENDOR_NAME="${LVFS_VENDOR_NAME:-System76}"
export LVFS_VENDOR_HOMEPAGE="${LVFS_VENDOR_HOMEPAGE:-https://system76.com/}"

invalid=0
if [[ -z "$LVFS_DEVICE_ID" ]]; then
    warn "LVFS_DEVICE_ID is not set"
    invalid=1
fi
if [[ -z "$LVFS_DEVICE_NAME" ]]; then
    warn "LVFS_DEVICE_NAME is not set"
    invalid=1
fi
if [[ -z "$LVFS_DEVICE_UUID" ]]; then
    warn "LVFS_DEVICE_UUID is not set"
    invalid=1
fi

GIT_COMMIT_HASH=$(git describe --always --abbrev=12 --exclude='*')
GIT_COMMIT_URL="https://github.com/system76/ec/tree/${GIT_COMMIT_HASH}"

GIT_COMMIT_RELEASE=$(git describe --always --dirty --abbrev=7 --exclude='*')
GIT_COMMIT_DATE=$(git show --format="%cd" --date="format:%Y-%m-%d" --no-patch)
EC_VERSION="${GIT_COMMIT_DATE}_${GIT_COMMIT_RELEASE}"

export LVFS_RELEASE_VERSION="${LVFS_RELEASE_VERSION:-${EC_VERSION}}"
export LVFS_RELEASE_DATE="${LVFS_RELEASE_DATE:-${GIT_COMMIT_DATE}}"
export LVFS_RELEASE_SOURCE_URL="${LVFS_RELEASE_SOURCE_URL:-${GIT_COMMIT_URL}}"

if [[ -z "$LVFS_RELEASE_DESCRIPTION" ]]; then
    warn "LVFS_RELEASE_DESCRIPTION is not set"
    invalid=1
fi

if [[ "$invalid" = "1" ]]; then
    error "Errors detected, not generating MetaInfo"
    exit 1
fi

envsubst < "$TEMPLATE"
