# SPDX-License-Identifier: GPL-3.0-only

-include config.mk

# Disable built-in rules and variables
MAKEFLAGS += -rR

# Default to silent builds
ifneq ($(VERBOSE),1)
MAKEFLAGS += -s
endif

# Parameter for current board
ifeq ($(BOARD),)
all:
	@echo -e "\x1B[31mBOARD must be specified\x1B[0m"
	@exit 1
else
# Calculate version
DATE=$(shell git show --format="%cs" --no-patch --no-show-signature)
REV=$(shell git describe --abbrev=7 --always --dirty)
VERSION?=$(DATE)_$(REV)

# Set build directory
obj = build
BUILD = $(obj)/$(BOARD)

# Default target - build the board's EC firmware
all: $(BUILD)/ec.rom
	$(info Built $(VERSION) for $(BOARD))

# Include common source
COMMON_DIR=src/common
INCLUDE += $(COMMON_DIR)/common.mk
CFLAGS=-I$(COMMON_DIR)/include -D__FIRMWARE_VERSION__=$(VERSION)
include $(COMMON_DIR)/common.mk
SRC += $(foreach src, $(common-y), $(COMMON_DIR)/$(src))

# Include the board's source
BOARD_DIR=src/board/$(BOARD)
INCLUDE += $(BOARD_DIR)/board.mk
CFLAGS+=-I$(BOARD_DIR)/include -D__BOARD__=$(BOARD)
include $(BOARD_DIR)/board.mk
SRC += $(foreach src, $(board-y), $(BOARD_DIR)/$(src))
SRC += $(foreach src, $(board-common-y), $(SYSTEM76_COMMON_DIR)/$(src))
SRC += $(foreach src, $(keyboard-y), $(KEYBOARD_DIR)/$(src))

# The board will define the embedded controller
# Include the embedded controller's source
EC_DIR=src/ec/$(EC)
INCLUDE += $(EC_DIR)/ec.mk
CFLAGS+=-I$(EC_DIR)/include
include $(EC_DIR)/ec.mk
SRC += $(foreach src, $(ec-y), $(EC_DIR)/$(src))

# The EC will define the architecture
# Include the architecture's source
ARCH_DIR=src/arch/$(ARCH)
INCLUDE += $(ARCH_DIR)/arch.mk
CFLAGS+=-I$(ARCH_DIR)/include -D__ARCH__=$(ARCH)
include $(ARCH_DIR)/arch.mk
SRC += $(foreach src, $(arch-y), $(ARCH_DIR)/$(src))

include $(ARCH_DIR)/toolchain.mk

# The architecture defines build targets, no more is required
endif

.PHONY: docs
docs:
	mdbook build docs/

.PHONY: clean
clean:
	rm -rf $(obj)

.PHONY: fmt
fmt:
	./scripts/clang-format.sh apply

.PHONY: lint
lint:
	./scripts/lint/lint.sh

.PHONY: list-boards
list-boards:
	@cd src/board && for board in */*/board.mk; do \
		echo "$$(dirname "$$board")"; \
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
