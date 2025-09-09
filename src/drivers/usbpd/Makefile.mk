# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: 2025 System76, Inc.

# USB Power Delivery (USB-PD) support.
#
# Options:
# - `CONFIG_USBPD_I2C`: Board may override the I2C device index to use.

ifeq ($(CONFIG_DRIVERS_USBPD_TPS65987),y)
CONFIG_HAVE_USBPD = y
endif

ifeq ($(CONFIG_HAVE_USBPD),y)

CFLAGS += -DCONFIG_HAVE_USBPD=1

ifeq ($(CONFIG_USBPD_I2C),)
CONFIG_USBPD_I2C = 1
endif

CFLAGS += -DUSBPD_I2C=I2C_$(CONFIG_USBPD_I2C)

drivers-$(CONFIG_DRIVERS_USBPD_TPS65987) += tps65987.c

endif
