# SPDX-License-Identifier: GPL-3.0-only

# SRAM is 4096 bytes, but SRAM at address 2048 is used for scratch ROM
SRAM_SIZE=2048

include src/ec/ite/ec.mk
