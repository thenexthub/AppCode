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

#ifndef APPCODE_FML_POSIX_WRAPPERS_H_
#define APPCODE_FML_POSIX_WRAPPERS_H_

#include "appcode/fml/build_config.h"

// Provides wrappers for POSIX functions that have been renamed on Windows.
// See
// https://docs.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-3-c4996?view=vs-2019#posix-function-names
// for context.
namespace fml {

char* strdup(const char* str1);

}  // namespace fml

#endif  // APPCODE_FML_POSIX_WRAPPERS_H_
