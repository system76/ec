# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: 2021 System76, Inc.

KEYMAP?=default
keyboard-y += keymap/$(KEYMAP).c

INCLUDE += $(KEYBOARD_DIR)/Makefile.mk
CFLAGS+=-I$(KEYBOARD_DIR)/include
