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

#ifndef APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_LAYERS_H_
#define APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_LAYERS_H_

#include <memory>
#include <vector>

#include "appcode/flow/embedded_views.h"
#include "appcode/fml/macros.h"
#include "appcode/shell/platform/embedder/embedder.h"
#include "third_party/skia/include/core/SkMatrix.h"
#include "third_party/skia/include/core/SkSize.h"

namespace appcode {

class EmbedderLayers {
 public:
  EmbedderLayers(SkISize frame_size,
                 double device_pixel_ratio,
                 SkMatrix root_surface_transformation,
                 uint64_t presentation_time);

  ~EmbedderLayers();

  void PushBackingStoreLayer(const FlutterBackingStore* store,
                             const std::vector<DlIRect>& drawn_region);

  void PushPlatformViewLayer(FlutterPlatformViewIdentifier identifier,
                             const EmbeddedViewParams& params);

  using PresentCallback =
      std::function<bool(FlutterViewId view_id,
                         const std::vector<const FlutterLayer*>& layers)>;
  void InvokePresentCallback(FlutterViewId view_id,
                             const PresentCallback& callback) const;

 private:
  const SkISize frame_size_;
  const double device_pixel_ratio_;
  const SkMatrix root_surface_transformation_;
  std::vector<std::unique_ptr<FlutterPlatformView>> platform_views_referenced_;
  std::vector<std::unique_ptr<FlutterPlatformViewMutation>>
      mutations_referenced_;
  std::vector<std::unique_ptr<std::vector<const FlutterPlatformViewMutation*>>>
      mutations_arrays_referenced_;
  std::vector<std::unique_ptr<FlutterBackingStorePresentInfo>>
      present_info_referenced_;
  std::vector<std::unique_ptr<FlutterRegion>> regions_referenced_;
  std::vector<std::unique_ptr<std::vector<FlutterRect>>> rects_referenced_;
  std::vector<FlutterLayer> presented_layers_;
  uint64_t presentation_time_;

  FML_DISALLOW_COPY_AND_ASSIGN(EmbedderLayers);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_LAYERS_H_
