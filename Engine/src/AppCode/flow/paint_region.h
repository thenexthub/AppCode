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

#ifndef APPCODE_FLOW_PAINT_REGION_H_
#define APPCODE_FLOW_PAINT_REGION_H_

#include <memory>
#include <utility>
#include <vector>

#include "appcode/display_list/geometry/dl_geometry_types.h"
#include "appcode/fml/logging.h"

namespace appcode {

// Corresponds to area on the screen where the layer subtree has painted to.
//
// The area is used when adding damage of removed or dirty layer to overall
// damage.
//
// Because there is a PaintRegion for each layer, it must be able to represent
// the area with minimal overhead. This is accomplished by having one
// vector<SkRect> shared between all paint regions, and each paint region
// keeping begin and end index of rects relevant to particular subtree.
//
// All rects are in screen coordinates.
class PaintRegion {
 public:
  PaintRegion() = default;
  PaintRegion(std::shared_ptr<std::vector<DlRect>> rects,
              size_t from,
              size_t to,
              bool has_readback,
              bool has_texture)
      : rects_(std::move(rects)),
        from_(from),
        to_(to),
        has_readback_(has_readback),
        has_texture_(has_texture) {}

  std::vector<DlRect>::const_iterator begin() const {
    FML_DCHECK(is_valid());
    return rects_->begin() + from_;
  }

  std::vector<DlRect>::const_iterator end() const {
    FML_DCHECK(is_valid());
    return rects_->begin() + to_;
  }

  // Compute bounds for this region
  DlRect ComputeBounds() const;

  bool is_valid() const { return rects_ != nullptr; }

  // Returns true if there is a layer in subtree represented by this region
  // that performs readback
  bool has_readback() const { return has_readback_; }

  // Returns whether there is a TextureLayer in subtree represented by this
  // region.
  bool has_texture() const { return has_texture_; }

 private:
  std::shared_ptr<std::vector<DlRect>> rects_;
  size_t from_ = 0;
  size_t to_ = 0;
  bool has_readback_ = false;
  bool has_texture_ = false;
};

}  // namespace appcode

#endif  // APPCODE_FLOW_PAINT_REGION_H_
