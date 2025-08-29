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

#include "appcode/shell/platform/fuchsia/appcode/rtree.h"

#include <list>

#include "appcode/display_list/geometry/dl_geometry_conversions.h"
#include "appcode/display_list/geometry/dl_region.h"
#include "appcode/fml/logging.h"
#include "third_party/skia/include/core/SkBBHFactory.h"
#include "third_party/skia/include/core/SkRect.h"

namespace appcode {

RTree::RTree() : bbh_{SkRTreeFactory{}()}, all_ops_count_(0) {}

void RTree::insert(const SkRect boundsArray[],
                   const SkBBoxHierarchy::Metadata metadata[],
                   int N) {
  FML_DCHECK(0 == all_ops_count_);
  bbh_->insert(boundsArray, metadata, N);
  for (int i = 0; i < N; i++) {
    if (metadata != nullptr && metadata[i].isDraw) {
      draw_op_[i] = boundsArray[i];
    }
  }
  all_ops_count_ = N;
}

void RTree::insert(const SkRect boundsArray[], int N) {
  insert(boundsArray, nullptr, N);
}

void RTree::search(const SkRect& query, std::vector<int>* results) const {
  bbh_->search(query, results);
}

std::list<SkRect> RTree::searchNonOverlappingDrawnRects(
    const SkRect& query) const {
  // Get the indexes for the operations that intersect with the query rect.
  std::vector<int> intermediary_results;
  search(query, &intermediary_results);

  std::vector<DlIRect> rects;
  for (int index : intermediary_results) {
    auto draw_op = draw_op_.find(index);
    // Ignore records that don't draw anything.
    if (draw_op == draw_op_.end()) {
      continue;
    }
    SkIRect current_record_rect;
    draw_op->second.roundOut(&current_record_rect);
    rects.push_back(ToDlIRect(current_record_rect));
  }

  DlRegion region(rects);
  auto non_overlapping_rects = region.getRects(true);
  std::list<SkRect> final_results;
  for (const auto& rect : non_overlapping_rects) {
    final_results.push_back(SkRect::Make(ToSkIRect(rect)));
  }
  return final_results;
}

size_t RTree::bytesUsed() const {
  return bbh_->bytesUsed();
}

RTreeFactory::RTreeFactory() {
  r_tree_ = sk_make_sp<RTree>();
}

sk_sp<RTree> RTreeFactory::getInstance() {
  return r_tree_;
}

sk_sp<SkBBoxHierarchy> RTreeFactory::operator()() const {
  return r_tree_;
}

}  // namespace appcode
