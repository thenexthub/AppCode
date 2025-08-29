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

#include "placeholder_run.h"

namespace txt {

PlaceholderRun::PlaceholderRun() {}

PlaceholderRun::PlaceholderRun(double width,
                               double height,
                               PlaceholderAlignment alignment,
                               TextBaseline baseline,
                               double baseline_offset)
    : width(width),
      height(height),
      alignment(alignment),
      baseline(baseline),
      baseline_offset(baseline_offset) {}

}  // namespace txt
