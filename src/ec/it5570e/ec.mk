# SPDX-License-Identifier: GPL-3.0-only

# SRAM is 6144 bytes, only 4096 bytes are mapped at address 0. Region at
# 0x0E00-0x1000 is used for AP communication. So this is brought down to 2048,
# which matches it8587e limits
SRAM_SIZE=2048

include src/ec/ite/ec.mk
