// SPDX-License-Identifier: GPL-3.0-only
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
