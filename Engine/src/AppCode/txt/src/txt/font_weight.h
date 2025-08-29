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

#ifndef APPCODE_TXT_SRC_TXT_FONT_WEIGHT_H_
#define APPCODE_TXT_SRC_TXT_FONT_WEIGHT_H_

namespace txt {

enum class FontWeight {
  // NOLINTBEGIN(readability-identifier-naming)
  w100,  // Thin
  w200,  // Extra-Light
  w300,  // Light
  w400,  // Normal/Regular
  w500,  // Medium
  w600,  // Semi-bold
  w700,  // Bold
  w800,  // Extra-Bold
  w900,  // Black
  // NOLINTEND(readability-identifier-naming)
};

}  // namespace txt

#endif  // APPCODE_TXT_SRC_TXT_FONT_WEIGHT_H_
