#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: 2021 System76, Inc.

spatch \
    --sp-file ./scripts/coccinelle/bit-macro.cocci \
    --macro-file ./scripts/coccinelle/macros.h \
    --no-includes \
    --include-headers \
    --preprocess \
    --in-place \
    --dir src/
