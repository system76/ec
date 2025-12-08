// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2021 System76, Inc.
// Replace shifts with BIT macro

@@ expression val; @@
-(1 << val)
+BIT(val)

@@ expression val; @@
-(1U << val)
+BIT(val)

@@ expression val; @@
-1 << val
+BIT(val)
