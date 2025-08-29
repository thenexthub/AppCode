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

#include "appcode/flow/layers/platform_view_layer.h"

#include "appcode/display_list/skia/dl_sk_canvas.h"

namespace appcode {

PlatformViewLayer::PlatformViewLayer(const DlPoint& offset,
                                     const DlSize& size,
                                     int64_t view_id)
    : offset_(offset), size_(size), view_id_(view_id) {}

void PlatformViewLayer::Preroll(PrerollContext* context) {
  set_paint_bounds(DlRect::MakeOriginSize(offset_, size_));

  if (context->view_embedder == nullptr) {
    FML_DLOG(ERROR) << "Trying to embed a platform view but the PrerollContext "
                       "does not support embedding";
    return;
  }
  context->has_platform_view = true;
  set_subtree_has_platform_view(true);
  MutatorsStack mutators;
  context->state_stack.fill(&mutators);
  std::unique_ptr<EmbeddedViewParams> params =
      std::make_unique<EmbeddedViewParams>(
          ToSkMatrix(context->state_stack.matrix()), ToSkSize(size_), mutators);
  context->view_embedder->PrerollCompositeEmbeddedView(view_id_,
                                                       std::move(params));
  context->view_embedder->PushVisitedPlatformView(view_id_);
}

void PlatformViewLayer::Paint(PaintContext& context) const {
  if (context.view_embedder == nullptr) {
    FML_DLOG(ERROR) << "Trying to embed a platform view but the PaintContext "
                       "does not support embedding";
    return;
  }
  DlCanvas* canvas = context.view_embedder->CompositeEmbeddedView(view_id_);
  context.canvas = canvas;
  context.state_stack.set_delegate(canvas);
  context.rendering_above_platform_view = true;
}

}  // namespace appcode
