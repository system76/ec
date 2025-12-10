# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: 2019 System76, Inc.

-include config.mk

# Disable built-in rules and variables
MAKEFLAGS += -rR

# Default to silent builds
ifneq ($(VERBOSE),1)
MAKEFLAGS += -s
endif

# Set build directory
BUILD = build

# Parameter for current board
ifeq ($(BOARD),)
all:
	$(error BOARD must be specified)
else
# Calculate version
DATE=$(shell git show --format="%cs" --no-patch --no-show-signature)
REV=$(shell git describe --abbrev=7 --always --dirty)
VERSION?=$(DATE)_$(REV)

# Default target - build the board's EC firmware
all: $(BUILD)/ec.rom
	$(info Built $(VERSION) for $(BOARD))

CFLAGS = -Isrc/

# Include common source
COMMON_DIR = src/common
INCLUDE += $(COMMON_DIR)/Makefile.mk
CFLAGS += -I$(COMMON_DIR)/include -D__FIRMWARE_VERSION__=$(VERSION)
include $(COMMON_DIR)/Makefile.mk
SRC += $(foreach src, $(common-y), $(COMMON_DIR)/$(src))

# Include the board's source
BOARD_DIR=src/board/$(BOARD)
INCLUDE += $(BOARD_DIR)/Makefile.mk
CFLAGS+=-I$(BOARD_DIR)/include -D__BOARD__=$(BOARD)
include $(BOARD_DIR)/Makefile.mk
SRC += $(foreach src, $(board-y), $(BOARD_DIR)/$(src))
SRC += $(foreach src, $(keyboard-y), $(KEYBOARD_DIR)/$(src))

include src/drivers/Makefile.mk

# Add System76 EC app
APP_DIR = src/app
INCLUDE += $(APP_DIR)/Makefile.mk
CFLAGS += -I$(APP_DIR)/include
include $(APP_DIR)/Makefile.mk
SRC += $(foreach src, $(app-y), $(APP_DIR)/$(src))

# The board will declare the SoC
SOC_DIR = src/soc/$(EC)
INCLUDE += $(SOC_DIR)/Makefile.mk
CFLAGS += -I$(SOC_DIR)/include
include $(SOC_DIR)/Makefile.mk
SRC += $(foreach src, $(soc-y), $(SOC_DIR)/$(src))

# The SoC will declare the architecture
ARCH_DIR = src/arch/$(ARCH)
INCLUDE += $(ARCH_DIR)/Makefile.mk
CFLAGS += -I$(ARCH_DIR)/include -D__ARCH__=$(ARCH)
include $(ARCH_DIR)/Makefile.mk
SRC += $(foreach src, $(arch-y), $(ARCH_DIR)/$(src))

include $(ARCH_DIR)/toolchain.mk

# TODO: Generate config file as part of build.
.PHONY: show-config
show-config:
	$(foreach v, \
		$(sort $(filter CONFIG_%,$(.VARIABLES))), \
		$(info $(v)=$($(v))))

# The architecture defines build targets, no more is required
endif

.PHONY: docs
docs:
	mdbook build docs/

.PHONY: clean
clean:
	rm -rf $(BUILD)

.PHONY: fmt
fmt:
	uncrustify -c .uncrustify.cfg -q --no-backup $(shell git ls-files '*.c' '*.h')

.PHONY: lint
lint:
	./scripts/lint/lint.sh

# TODO: Move "common" out of board directory
.PHONY: list-boards
list-boards:
	@cd src/board && for board in */*/Makefile.mk; do \
		if [ "$$board" != "system76/common/Makefile.mk" ]; then \
			echo "$$(dirname "$$board")"; \
		fi; \
	done

# This target is run during setup, and is not shown in the help text.
.PHONY: git-config
git-config:
	$(eval HOOKS = "$(shell git rev-parse --git-dir)/hooks")
	ln -sfrv scripts/hooks/pre-commit.sh "$(HOOKS)/pre-commit"

.PHONY: help
help:
	@echo "System76 EC build targets"
	@echo "    [all] BOARD=<B>      Build the specified board"
	@echo "    list-boards          List available target boards"
	@echo "    clean                Remove build artifacts"
	@echo "    fmt                  Format the source code"
	@echo "    lint                 Run lint checks"
	@echo "    help                 Print this message"
