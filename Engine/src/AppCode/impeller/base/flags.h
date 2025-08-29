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

#ifndef APPCODE_IMPELLER_BASE_FLAGS_H_
#define APPCODE_IMPELLER_BASE_FLAGS_H_

namespace impeller {
struct Flags {
  /// Whether to defer PSO construction until first use. Usage Will introduce
  /// raster jank.
  bool lazy_shader_mode = false;
  /// When turned on DrawLine will use the experimental antialiased path.
  bool antialiased_lines = false;
};
}  // namespace impeller

#endif  // APPCODE_IMPELLER_BASE_FLAGS_H_
