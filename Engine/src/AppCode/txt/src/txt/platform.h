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

#ifndef APPCODE_TXT_SRC_TXT_PLATFORM_H_
#define APPCODE_TXT_SRC_TXT_PLATFORM_H_

#include <string>
#include <vector>

#include "appcode/fml/macros.h"
#include "third_party/skia/include/core/SkFontMgr.h"

namespace txt {

std::vector<std::string> GetDefaultFontFamilies();

sk_sp<SkFontMgr> GetDefaultFontManager(uint32_t font_initialization_data = 0);

}  // namespace txt

#endif  // APPCODE_TXT_SRC_TXT_PLATFORM_H_
