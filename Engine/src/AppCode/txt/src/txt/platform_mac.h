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

#ifndef APPCODE_TXT_SRC_TXT_PLATFORM_MAC_H_
#define APPCODE_TXT_SRC_TXT_PLATFORM_MAC_H_

#include "txt/asset_font_manager.h"

namespace txt {

// Register additional system font for MacOS and iOS.
void RegisterSystemFonts(const DynamicFontManager& dynamic_font_manager);

}  // namespace txt

#endif  // APPCODE_TXT_SRC_TXT_PLATFORM_MAC_H_
