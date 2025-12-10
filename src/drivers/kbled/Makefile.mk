# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: 2025 System76, Inc.

# Keyboard LED backlight support.
#
# ### DAC
#
# Requires:
# - Board must declare the DAC channel to use.
#
# ### I2C
#
# Requires:
# - Board must declare the I2C device index to use.
# - Board must select the color channel layout to use.
#
# ### PWM
#
# Assumes:
# - DCR0 is used for brightness channel.
# - DCR{5,6,7} is used for color channels.
#
# ### USB HID
#
# Requires:
# - Board must declare the I2C device index to use.

ifeq ($(CONFIG_DRIVERS_KBLED_DAC)$(CONFIG_DRIVERS_KBLED_I2C)$(CONFIG_DRIVERS_KBLED_PWM)$(CONFIG_DRIVERS_KBLED_USB_HID),y)
CONFIG_HAVE_KBLED = y
endif

ifeq ($(CONFIG_HAVE_KBLED),y)

CFLAGS += -DCONFIG_HAVE_KBLED=1

drivers-y += hotkey.c

# DAC
ifneq ($(CONFIG_DRIVERS_KBLED_DAC),)
CFLAGS += -DKBLED_DAC=$(CONFIG_KBLED_DAC_CH)

drivers-y += dac.c
endif

# I2C
ifneq ($(CONFIG_DRIVERS_KBLED_I2C),)
CFLAGS += -DKBLED_I2C=I2C_$(CONFIG_KBLED_I2C)
ifeq ($(CONFIG_KBLED_I2C_CH_BGR_PACKED),y)
CFLAGS += -DCONFIG_KBLED_I2C_CH_BGR_PACKED=1
else ifeq ($(CONFIG_KBLED_I2C_CH_BGR_MIXED),y)
CFLAGS += -DCONFIG_KBLED_I2C_CH_BGR_MIXED=1
endif

drivers-y += i2c.c
endif

# PWM
ifneq ($(CONFIG_DRIVERS_KBLED_PWM),)
drivers-y += pwm.c
endif

# USB HID
ifneq ($(CONFIG_DRIVERS_KBLED_USB_HID),)
CFLAGS += -DKBLED_I2C=I2C_$(CONFIG_KBLED_I2C)

drivers-y += usb.c
endif

endif
