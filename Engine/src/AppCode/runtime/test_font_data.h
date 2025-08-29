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

#ifndef APPCODE_RUNTIME_TEST_FONT_DATA_H_
#define APPCODE_RUNTIME_TEST_FONT_DATA_H_

#include <memory>
#include <string>
#include <vector>

#include "third_party/skia/include/core/SkStream.h"
#include "third_party/skia/include/core/SkTypeface.h"

namespace appcode {

std::vector<sk_sp<SkTypeface>> GetTestFontData();
std::vector<std::string> GetTestFontFamilyNames();

}  // namespace appcode

#endif  // APPCODE_RUNTIME_TEST_FONT_DATA_H_
