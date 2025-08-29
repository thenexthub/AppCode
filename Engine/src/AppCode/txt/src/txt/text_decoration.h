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

#ifndef APPCODE_TXT_SRC_TXT_TEXT_DECORATION_H_
#define APPCODE_TXT_SRC_TXT_TEXT_DECORATION_H_

namespace txt {

// Multiple decorations can be applied at once. Ex: Underline and overline is
// (0x1 | 0x2)
enum TextDecoration {
  kNone = 0x0,
  kUnderline = 0x1,
  kOverline = 0x2,
  kLineThrough = 0x4,
};

enum TextDecorationStyle { kSolid, kDouble, kDotted, kDashed, kWavy };

}  // namespace txt

#endif  // APPCODE_TXT_SRC_TXT_TEXT_DECORATION_H_
