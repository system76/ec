# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: 2024 System76, Inc.

# Disable built-in rules and variables
MAKEFLAGS += --no-builtin-rules --no-builtin-variables
.SUFFIXES:

# Default to silent builds
ifneq ($(VERBOSE),1)
MAKEFLAGS += --silent
.SILENT:
endif

PODMAN := $(shell command -v podman)
CONTAINER_TAG := $(shell git describe --always --abbrev=12)

.PHONY: ec
ec:
	$(PODMAN) build \
		--tag system76/$@:$(CONTAINER_TAG) \
		--file Containerfile \
		$@
