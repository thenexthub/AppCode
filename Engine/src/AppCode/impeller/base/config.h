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

#ifndef APPCODE_IMPELLER_BASE_CONFIG_H_
#define APPCODE_IMPELLER_BASE_CONFIG_H_

#include <cstdlib>

#include "appcode/fml/logging.h"

#if defined(__GNUC__) || defined(__clang__)
#define IMPELLER_COMPILER_CLANG 1
#else  // defined(__GNUC__) || defined(__clang__)
#define IMPELLER_COMPILER_CLANG 0
#endif  // defined(__GNUC__) || defined(__clang__)

#if IMPELLER_COMPILER_CLANG
#define IMPELLER_PRINTF_FORMAT(format_number, args_number) \
  __attribute__((format(printf, format_number, args_number)))
#else  // IMPELLER_COMPILER_CLANG
#define IMPELLER_PRINTF_FORMAT(format_number, args_number)
#endif  // IMPELLER_COMPILER_CLANG

#define IMPELLER_UNIMPLEMENTED \
  impeller::ImpellerUnimplemented(__FUNCTION__, __FILE__, __LINE__);

namespace impeller {

[[noreturn]] inline void ImpellerUnimplemented(const char* method,
                                               const char* file,
                                               int line) {
  FML_CHECK(false) << "Unimplemented: " << method << " in " << file << ":"
                   << line;
  std::abort();
}

}  // namespace impeller

#endif  // APPCODE_IMPELLER_BASE_CONFIG_H_
