//===----------------------------------------------------------------------===//
//
// Copyright (c) 2025 NeXTHub Corporation. All rights reserved.
// DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
//
// This code is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// version 2 for more details (a copy is included in the LICENSE file that
// accompanied this code).
//
// Author(-s): Tunjay Akbarli
// Creation Date: Saturday, May 10, 2025.
//
//===----------------------------------------------------------------------===//

#ifndef APPCODE_SHELL_PLATFORM_LINUX_TESTING_FL_TEST_H_
#define APPCODE_SHELL_PLATFORM_LINUX_TESTING_FL_TEST_H_

#include "appcode/shell/platform/linux/public/appcode_linux/fl_engine.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_value.h"

#include <glib.h>
#include <stdint.h>
#include <ostream>

G_BEGIN_DECLS

// Helper functions for the tests. This is not included in the shell library.

// Helper function to convert a hexadecimal string (e.g. "01feab") into GBytes
GBytes* hex_string_to_bytes(const gchar* hex_string);

// Helper function to convert GBytes into a hexadecimal string (e.g. "01feab")
gchar* bytes_to_hex_string(GBytes* bytes);

// GTest printer for FlValue.
void PrintTo(FlValue* v, std::ostream* os);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_TESTING_FL_TEST_H_
