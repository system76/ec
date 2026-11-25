# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: 2026 System76, Inc.

# Platform Environment Control Interface (PECI)
#
# Requires:
# - `CONFIG_PLATFORM_INTEL`: Intel proprietary interface

ifeq ($(CONFIG_PLATFORM_INTEL),y)

drivers-y += peci.c

endif
