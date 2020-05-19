# SPDX-License-Identifier: GPL-3.0-only

ARCH=8051

# 64 KB is the max without banking
CODE_SIZE=65536

# SRAM is 4096 bytes, but SRAM at address 2048 is used for scratch ROM
SRAM_SIZE=2048
