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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_RTREE_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_RTREE_H_

#include <list>
#include <map>

#include "third_party/skia/include/core/SkBBHFactory.h"
#include "third_party/skia/include/core/SkTypes.h"

namespace appcode {
/**
 * An R-Tree implementation that forwards calls to an SkRTree.
 *
 * This implementation provides a searchNonOverlappingDrawnRects method,
 * which can be used to query the rects for the operations recorded in the tree.
 */
class RTree : public SkBBoxHierarchy {
 public:
  RTree();

  void insert(const SkRect[],
              const SkBBoxHierarchy::Metadata[],
              int N) override;
  void insert(const SkRect[], int N) override;
  void search(const SkRect& query, std::vector<int>* results) const override;
  size_t bytesUsed() const override;

  // Finds the rects in the tree that represent drawing operations and intersect
  // with the query rect.
  //
  // When two rects intersect with each other, they are joined into a single
  // rect which also intersects with the query rect. In other words, the bounds
  // of each rect in the result list are mutually exclusive.
  std::list<SkRect> searchNonOverlappingDrawnRects(const SkRect& query) const;

  // Insertion count (not overall node count, which may be greater).
  int getCount() const { return all_ops_count_; }

 private:
  // A map containing the draw operation rects keyed off the operation index
  // in the insert call.
  std::map<int, SkRect> draw_op_;
  sk_sp<SkBBoxHierarchy> bbh_;
  int all_ops_count_;
};

class RTreeFactory : public SkBBHFactory {
 public:
  RTreeFactory();

  // Gets the instance to the R-tree.
  sk_sp<RTree> getInstance();
  sk_sp<SkBBoxHierarchy> operator()() const override;

 private:
  sk_sp<RTree> r_tree_;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_RTREE_H_
