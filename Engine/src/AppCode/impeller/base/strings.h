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

#ifndef APPCODE_IMPELLER_BASE_STRINGS_H_
#define APPCODE_IMPELLER_BASE_STRINGS_H_

#include <string>

#include "impeller/base/config.h"

namespace impeller {

IMPELLER_PRINTF_FORMAT(1, 2)
std::string SPrintF(const char* format, ...);

bool HasPrefix(const std::string& string, const std::string& prefix);

bool HasSuffix(const std::string& string, const std::string& suffix);

std::string StripPrefix(const std::string& string, const std::string& to_strip);

}  // namespace impeller

#endif  // APPCODE_IMPELLER_BASE_STRINGS_H_
