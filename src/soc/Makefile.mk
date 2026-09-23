# SPDX-License-Identifier: GPL-3.0-only

ifeq ($(CONFIG_SOC_ITE_IT8587E)$(CONFIG_SOC_ITE_IT5570E)$(CONFIG_SOC_ITE_5571E),y)
CONFIG_SOC_VENDOR = ite
endif

ifeq ($(CONFIG_SOC_VENDOR),)
$(error SoC selection misconfigured)
endif

SOC_DIR = src/soc/$(CONFIG_SOC_VENDOR)

INCLUDE += $(SOC_DIR)/Makefile.mk
CFLAGS += -I$(SOC_DIR)/include
include $(SOC_DIR)/Makefile.mk
SRC += $(foreach src,$(soc-y),$(SOC_DIR)/$(src))
