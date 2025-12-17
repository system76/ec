# SPDX-License-Identifier: GPL-3.0-only

# Disable built-in rules and variables
MAKEFLAGS += --no-builtin-rules --no-builtin-variables
.SUFFIXES:

# Default to silent builds
ifneq ($(VERBOSE),1)
MAKEFLAGS += --silent
.SILENT:
endif

PODMAN := $(shell command -v podman)

.PHONY: all
all: sdcc ec

# SDCC toolchain container

SDCC_REPO := https://svn.code.sf.net/p/sdcc/code
SDCC_REV := 14648
SDCC_VERSION := 4.4.0

.PHONY: sdcc
sdcc:
	$(PODMAN) build \
		--tag system76/$@:$(SDCC_VERSION) \
		--build-arg=SDCC_REPO="$(SDCC_REPO)" \
		--build-arg=SDCC_REV="$(SDCC_REV)" \
		--build-arg=SDCC_VERSION="$(SDCC_VERSION)" \
		--file Containerfile \
		$@

# EC development container

DEVCONTAINER_TAG := $(shell git describe --always --abbrev=12)
RUST_TOOLCHAIN := 1.85.0

.PHONY: ec
ec:
	$(PODMAN) build \
		--tag system76/$@:$(DEVCONTAINER_TAG) \
		--build-arg=RUST_TOOLCHAIN="$(RUST_TOOLCHAIN)" \
		--file Containerfile \
		$@
