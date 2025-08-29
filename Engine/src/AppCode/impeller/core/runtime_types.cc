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

#include "impeller/core/runtime_types.h"

namespace impeller {

size_t RuntimeUniformDescription::GetSize() const {
  size_t size = dimensions.rows * dimensions.cols * bit_width / 8u;
  if (array_elements.value_or(0) > 0) {
    // Covered by check on the line above.
    // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
    size *= array_elements.value();
  }
  size += sizeof(float) * struct_layout.size();
  return size;
}

}  // namespace impeller
