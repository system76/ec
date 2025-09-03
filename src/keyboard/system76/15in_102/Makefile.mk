# SPDX-License-Identifier: GPL-3.0-only

KEYMAP?=default
keyboard-y += keymap/$(KEYMAP).c

INCLUDE += $(KEYBOARD_DIR)/Makefile.mk
CFLAGS+=-I$(KEYBOARD_DIR)/include
