# SPDX-License-Identifier: GPL-3.0-only

# Disable built-in rules and variables
MAKEFLAGS += -rR

PODMAN := $(shell command -v podman)

SDCC_REPO := https://svn.code.sf.net/p/sdcc/code
SDCC_REV := 14648
SDCC_VERSION := 4.4.0

.PHONY: ec-alpine
ec-alpine:
	$(PODMAN) build \
		--tag system76/$@:latest \
		--build-arg=SDCC_REPO="$(SDCC_REPO)" \
		--build-arg=SDCC_REV="$(SDCC_REV)" \
		--build-arg=SDCC_VERSION="$(SDCC_VERSION)" \
		--file Containerfile \
		$@

.PHONY: ec-debian
ec-debian:
	$(PODMAN) build \
		--tag system76/$@:latest \
		--build-arg=SDCC_REPO="$(SDCC_REPO)" \
		--build-arg=SDCC_REV="$(SDCC_REV)" \
		--build-arg=SDCC_VERSION="$(SDCC_VERSION)" \
		--file Containerfile \
		$@
