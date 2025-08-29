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

#ifndef APPCODE_FLOW_TESTING_DIFF_CONTEXT_TEST_H_
#define APPCODE_FLOW_TESTING_DIFF_CONTEXT_TEST_H_

#include <utility>

#include "appcode/display_list/geometry/dl_geometry_types.h"
#include "appcode/flow/layers/container_layer.h"
#include "appcode/flow/layers/display_list_layer.h"
#include "appcode/flow/layers/opacity_layer.h"
#include "appcode/flow/testing/layer_test.h"

namespace appcode {
namespace testing {

class MockLayerTree {
 public:
  explicit MockLayerTree(DlISize size = DlISize(1000, 1000))
      : root_(std::make_shared<ContainerLayer>()), size_(size) {}

  ContainerLayer* root() { return root_.get(); }
  const ContainerLayer* root() const { return root_.get(); }

  PaintRegionMap& paint_region_map() { return paint_region_map_; }
  const PaintRegionMap& paint_region_map() const { return paint_region_map_; }

  const DlISize& size() const { return size_; }

 private:
  std::shared_ptr<ContainerLayer> root_;
  PaintRegionMap paint_region_map_;
  DlISize size_;
};

class DiffContextTest : public LayerTest {
 public:
  DiffContextTest();

  Damage DiffLayerTree(MockLayerTree& layer_tree,
                       const MockLayerTree& old_layer_tree,
                       const DlIRect& additional_damage = DlIRect(),
                       int horizontal_clip_alignment = 0,
                       int vertical_alignment = 0,
                       bool use_raster_cache = true,
                       bool impeller_enabled = false);

  // Create display list consisting of filled rect with given color; Being able
  // to specify different color is useful to test deep comparison of pictures
  sk_sp<DisplayList> CreateDisplayList(const DlRect& bounds,
                                       DlColor color = DlColor::kBlack());

  std::shared_ptr<DisplayListLayer> CreateDisplayListLayer(
      const sk_sp<DisplayList>& display_list,
      const DlPoint& offset = DlPoint(0, 0));

  std::shared_ptr<ContainerLayer> CreateContainerLayer(
      std::initializer_list<std::shared_ptr<Layer>> layers);

  std::shared_ptr<ContainerLayer> CreateContainerLayer(
      std::shared_ptr<Layer> l) {
    return CreateContainerLayer({std::move(l)});
  }

  std::shared_ptr<OpacityLayer> CreateOpacityLater(
      std::initializer_list<std::shared_ptr<Layer>> layers,
      uint8_t alpha,
      const DlPoint& offset = DlPoint(0, 0));
};

}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_FLOW_TESTING_DIFF_CONTEXT_TEST_H_
