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

#ifndef APPCODE_DISPLAY_LIST_EFFECTS_COLOR_SOURCES_DL_MATRIX_COLOR_SOURCE_BASE_H_
#define APPCODE_DISPLAY_LIST_EFFECTS_COLOR_SOURCES_DL_MATRIX_COLOR_SOURCE_BASE_H_

#include "appcode/display_list/effects/dl_color_source.h"

namespace appcode {

// Utility base class common to all DlColorSource implementations that
// hold an optional DlMatrix
class DlMatrixColorSourceBase : public DlColorSource {
 public:
  const DlMatrix& matrix() const { return matrix_; }
  const DlMatrix* matrix_ptr() const {
    return matrix_.IsIdentity() ? nullptr : &matrix_;
  }

 protected:
  explicit DlMatrixColorSourceBase(const DlMatrix* matrix)
      : matrix_(matrix ? *matrix : DlMatrix()) {}

 private:
  const DlMatrix matrix_;
};

}  // namespace appcode

#endif  // APPCODE_DISPLAY_LIST_EFFECTS_COLOR_SOURCES_DL_MATRIX_COLOR_SOURCE_BASE_H_
