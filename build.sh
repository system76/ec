#!/usr/bin/env bash

ROOT_DIR=$(dirname $(readlink -f ${BASH_SOURCE[0]}))

errorExit() {
    errorMessage="$1"
    echo "$errorMessage"
    exit 1
}

errorCheck() {
    errorCode=$?
    errorMessage="$1"
    [ "$errorCode" -ne 0 ] && errorExit "$errorMessage : ($errorCode)"
    return $errorCode
}

usage() {
cat <<EOF
Usage: ./$(basename ${0})

  Environmental variables:
    EC_BOARD_VENDOR        name of the board vendor (required)
    EC_BOARD_MODEL         name of the board model (required)

Example usage to build for NovaCustom NV40 series:
    EC_BOARD_VENDOR=clevo EC_BOARD_MODEL=nv40mz ./$(basename $0)
EOF
  exit 0
}

if [ -z "$EC_BOARD_VENDOR" ]; then
  echo "EC_BOARD_VENDOR not given"
  usage
fi

if [ -z "$EC_BOARD_MODEL" ]; then
  echo "EC_BOARD_MODEL not given"
  usage
fi

EC_GIT_DATE="$(git show -s --format=%cs HEAD)"
EC_GIT_REV="$(git rev-parse --short HEAD)"
EC_BUILD_DIR="build/${EC_BOARD_VENDOR}/${EC_BOARD_MODEL}/${EC_GIT_DATE}_${EC_GIT_REV:0:7}"
EC_ROM="${EC_BUILD_DIR}/ec.rom"
EC_ARTIFACT="${EC_BOARD_VENDOR}_${EC_BOARD_MODEL}_ec.rom"

docker run --rm -it -v "$PWD":"$PWD" -w "$PWD" \
  ghcr.io/dasharo/ec-sdk make BOARD=${EC_BOARD_VENDOR}/${EC_BOARD_MODEL}
errorCheck "Failed to build EC fimware"

cp "$EC_ROM" "$EC_ARTIFACT"
errorCheck "Failed to rename EC firmware"

dd if=/dev/zero of=$EC_ARTIFACT bs=1 seek=128k count=0
errorCheck "Failed to extend EC firmware"
