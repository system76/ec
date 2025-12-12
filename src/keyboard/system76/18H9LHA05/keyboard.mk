# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: 2023 System76, Inc.

KEYMAP?=default
keyboard-y += keymap/$(KEYMAP).c

INCLUDE += $(KEYBOARD_DIR)/keyboard.mk
CFLAGS+=-I$(KEYBOARD_DIR)/include
