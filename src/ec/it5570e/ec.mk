# SPDX-License-Identifier: GPL-3.0-only

ARCH=8051

# 64 KB is the max without banking
CODE_SIZE=65536

# SRAM is 6144 bytes, only 4096 bytes are mapped at address 0. Region at
# 0x0E00-0x1000 is used for AP communication. So this is brought down to 2048,
# which matches it8587e limits
SRAM_SIZE=2048
