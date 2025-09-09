# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: 2025 System76, Inc.

# Discrete GPU (dGPU) support.
#
# Options:
# - `CONFIG_DGPU_I2C`: Board may override the I2C device index to use.

ifeq ($(CONFIG_DRIVERS_DGPU_NVIDIA),y)
CONFIG_HAVE_DGPU = y
endif

ifeq ($(CONFIG_HAVE_DGPU),y)

CFLAGS += -DCONFIG_HAVE_DGPU=1

ifeq ($(CONFIG_DGPU_I2C),)
CONFIG_DGPU_I2C = 1
endif

CFLAGS += -DDGPU_I2C=I2C_$(CONFIG_DGPU_I2C)

drivers-$(CONFIG_DRIVERS_DGPU_NVIDIA) += nvidia.c

endif
