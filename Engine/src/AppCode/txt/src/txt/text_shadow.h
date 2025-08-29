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

#ifndef APPCODE_TXT_SRC_TXT_TEXT_SHADOW_H_
#define APPCODE_TXT_SRC_TXT_TEXT_SHADOW_H_

#include "third_party/skia/include/core/SkColor.h"
#include "third_party/skia/include/core/SkPoint.h"

namespace txt {

class TextShadow {
 public:
  SkColor color = SK_ColorBLACK;
  SkPoint offset;
  double blur_sigma = 0.0;

  TextShadow();

  TextShadow(SkColor color, SkPoint offset, double blur_sigma);

  bool operator==(const TextShadow& other) const;

  bool operator!=(const TextShadow& other) const;

  bool hasShadow() const;
};

}  // namespace txt

#endif  // APPCODE_TXT_SRC_TXT_TEXT_SHADOW_H_
