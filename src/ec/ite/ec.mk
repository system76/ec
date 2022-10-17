# SPDX-License-Identifier: GPL-3.0-only

ec-y += ec.c
ec-y += espi.c
ec-y += gpio.c
ec-y += i2c.c
ec-y += kbc.c
ec-y += pmc.c
ec-y += ps2.c
ec-y += signature.c

ifeq ($(CONFIG_EC_ITE_IT8587E), y)
CFLAGS+=-DCONFIG_EC_ITE_IT8587E=1
# SRAM is 4096 bytes, but SRAM at address 2048 is used for scratch ROM
SRAM_SIZE=2048
else ifeq ($(CONFIG_EC_ITE_IT5570E), y)
CFLAGS+=-DCONFIG_EC_ITE_IT5570E=1
# SRAM is 6144 bytes, only 4096 bytes are mapped at address 0. Region at
# 0x0E00-0x1000 is used for AP communication. So this is brought down to 2048,
# which matches IT8587E limits
SRAM_SIZE=2048
else
$(error Unsupported EC)
endif

ARCH=8051

# 64 KB is the max without banking
CODE_SIZE=65536
