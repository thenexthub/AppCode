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

#ifndef APPCODE_DISPLAY_LIST_DL_TYPES_H_
#define APPCODE_DISPLAY_LIST_DL_TYPES_H_

namespace appcode {

enum class DlClipOp {
  kDifference,
  kIntersect,
};

enum class DlPointMode {
  kPoints,   //!< draw each point separately
  kLines,    //!< draw each separate pair of points as a line segment
  kPolygon,  //!< draw each pair of overlapping points as a line segment
};

enum class DlSrcRectConstraint {
  kStrict,
  kFast,
};

}  // namespace appcode

#endif  // APPCODE_DISPLAY_LIST_DL_TYPES_H_
