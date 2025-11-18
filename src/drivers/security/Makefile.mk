# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: 2025 System76, Inc.

# Firmware security state feature.
#
# Requires:
# - Board must declare the `ME_WE` pin.
#
# External integrations:
# - system76/coreboot: `ec/system76/ec` config `EC_SYSTEM76_EC_LOCKDOWN`.
# - system76/firmware-setup: UEFI protocol for physical presence prompt.

ifeq ($(CONFIG_SECURITY),y)

CFLAGS += -DCONFIG_SECURITY=1

drivers-y += security.c

endif
