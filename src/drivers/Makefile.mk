# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: 2025 System76, Inc.

subdirs-y := $(wildcard src/drivers/*)

$(foreach subdir,$(subdirs-y), \
	$(if $(wildcard $(subdir)/Makefile.mk), \
		$(eval drivers-y:=) \
		$(eval include $(subdir)/Makefile.mk) \
		$(eval INCLUDE+=$(subdir)/Makefile.mk) \
		$(foreach src,$(drivers-y), \
			$(eval SRC+=$(addprefix $(subdir)/,$(src))))))
