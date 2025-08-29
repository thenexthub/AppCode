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

#ifndef APPCODE_DISPLAY_LIST_UTILS_DL_ACCUMULATION_RECT_H_
#define APPCODE_DISPLAY_LIST_UTILS_DL_ACCUMULATION_RECT_H_

#include "appcode/display_list/geometry/dl_geometry_types.h"

namespace appcode {

// Utility class to collect bounds from a bunch of rectangles and points
// while also noting if there might be any overlap between any of the data
// point/rects. Note that the overlap protection is not sophisticated,
// simply noting if the new data intersects with the already accumulated
// bounds. This can successfully detect non-overlap of a linear sequence
// of non-overlapping objects, or even a cross of non-overlapping objects
// as long as they are built out from the center in the right order. True
// detection of non-overlapping objects would require much more time and/or
// space.
class AccumulationRect {
 public:
  AccumulationRect() { reset(); }

  void accumulate(DlScalar x, DlScalar y);
  void accumulate(DlPoint p) { accumulate(p.x, p.y); }
  void accumulate(DlRect r);
  void accumulate(AccumulationRect& ar);

  bool is_empty() const { return min_x_ >= max_x_ || min_y_ >= max_y_; }
  bool is_not_empty() const { return min_x_ < max_x_ && min_y_ < max_y_; }

  DlRect GetBounds() const;

  void reset();

  bool overlap_detected() const { return overlap_detected_; }
  void record_overlapping_bounds() { overlap_detected_ = true; }

 private:
  DlScalar min_x_;
  DlScalar min_y_;
  DlScalar max_x_;
  DlScalar max_y_;
  bool overlap_detected_;
};

}  // namespace appcode

#endif  // APPCODE_DISPLAY_LIST_UTILS_DL_ACCUMULATION_RECT_H_
