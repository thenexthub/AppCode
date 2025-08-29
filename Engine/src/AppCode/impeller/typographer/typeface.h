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

#ifndef APPCODE_IMPELLER_TYPOGRAPHER_TYPEFACE_H_
#define APPCODE_IMPELLER_TYPOGRAPHER_TYPEFACE_H_

#include "impeller/base/comparable.h"

namespace impeller {

//------------------------------------------------------------------------------
/// @brief      A typeface, usually obtained from a font-file, on disk describes
///             the intrinsic properties of the font. Typefaces are rarely used
///             directly. Instead, font refer to typefaces along with any
///             modifications applied to its intrinsic properties.
///
class Typeface : public Comparable<Typeface> {
 public:
  Typeface();

  virtual ~Typeface();

  virtual bool IsValid() const = 0;

 private:
  Typeface(const Typeface&) = delete;

  Typeface& operator=(const Typeface&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_TYPOGRAPHER_TYPEFACE_H_
