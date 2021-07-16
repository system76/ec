# SPDX-License-Identifier: GPL-3.0-only

ARCH=8051

# 64 KB is the max without banking
CODE_SIZE=65536

# Include ITE common source
EC_ITE_COMMON_DIR=src/ec/ite
SRC+=$(wildcard $(EC_ITE_COMMON_DIR)/*.c)
INCLUDE+=$(wildcard $(EC_ITE_COMMON_DIR)/include/ec/*.h) $(EC_ITE_COMMON_DIR)/ec.mk
CFLAGS+=-I$(EC_ITE_COMMON_DIR)/include
