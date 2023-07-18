#!/usr/bin/env bash
# SPDX-License-Identifier: MIT

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
Usage: ./$(basename "${0}")

  Environmental variables:
    EC_BOARD_VENDOR        name of the board vendor (required)
    EC_BOARD_MODEL         name of the board model (required)

Example usage to build for NovaCustom NV40 series:
    EC_BOARD_VENDOR=clevo EC_BOARD_MODEL=nv40mz ./$(basename "$0")
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

# For already released boards, keep the original names (already present in the
# released firmwares) to avoid the necessity of force-flashing during EC
# firmware update:
# - clevo/nv40mz for novacustom/nv4x_tgl:
#   - https://docs.dasharo.com/variants/novacustom_nv4x_tgl/releases/#v130-2022-10-18
# - clevo/ns50mu for novacustom/ns5x_tgl:
#   - https://docs.dasharo.com/variants/novacustom_ns5x_tgl/releases/#v130-2022-09-01
#

if [ "$EC_BOARD_VENDOR" = "novacustom" ] ; then
  mkdir -p build/novacustom/
  case "$EC_BOARD_MODEL" in
  "nv4x_tgl")
    ln -s ../clevo/nv40mz build/novacustom/nv4x_tgl
    EC_BOARD_VENDOR="clevo"
    EC_BOARD_MODEL="nv40mz"
    ;;
  "ns5x_tgl")
    ln -s ../clevo/ns50mu build/novacustom/ns5x_tgl
    EC_BOARD_VENDOR="clevo"
    EC_BOARD_MODEL="ns50mu"
    ;;
  *)
  esac
fi

docker run --rm -v "$PWD":"$PWD" -w "$PWD" -u "$(id -u)" \
  ghcr.io/dasharo/ec-sdk:main make BOARD=${EC_BOARD_VENDOR}/${EC_BOARD_MODEL}
errorCheck "Failed to build EC fimware"

cp "$EC_ROM" "$EC_ARTIFACT"
errorCheck "Failed to rename EC firmware"

dd if=/dev/zero of="$EC_ARTIFACT" bs=1 seek=128k count=0
errorCheck "Failed to extend EC firmware"
