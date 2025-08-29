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

#include "appcode/shell/platform/embedder/embedder_render_target_skia.h"

#include "appcode/fml/logging.h"

namespace appcode {

EmbedderRenderTargetSkia::EmbedderRenderTargetSkia(
    appcodeBackingStore backing_store,
    sk_sp<SkSurface> render_surface,
    fml::closure on_release,
    MakeOrClearCurrentCallback on_make_current,
    MakeOrClearCurrentCallback on_clear_current)
    : EmbedderRenderTarget(backing_store, std::move(on_release)),
      render_surface_(std::move(render_surface)),
      on_make_current_(std::move(on_make_current)),
      on_clear_current_(std::move(on_clear_current)) {
  FML_DCHECK(render_surface_);
}

EmbedderRenderTargetSkia::~EmbedderRenderTargetSkia() = default;

sk_sp<SkSurface> EmbedderRenderTargetSkia::GetSkiaSurface() const {
  return render_surface_;
}

impeller::RenderTarget* EmbedderRenderTargetSkia::GetImpellerRenderTarget()
    const {
  return nullptr;
}

std::shared_ptr<impeller::AiksContext>
EmbedderRenderTargetSkia::GetAiksContext() const {
  return nullptr;
}

SkISize EmbedderRenderTargetSkia::GetRenderTargetSize() const {
  return SkISize::Make(render_surface_->width(), render_surface_->height());
}

EmbedderRenderTarget::SetCurrentResult
EmbedderRenderTargetSkia::MaybeMakeCurrent() const {
  if (on_make_current_ != nullptr) {
    return on_make_current_();
  }

  return {true, false};
}

EmbedderRenderTarget::SetCurrentResult
EmbedderRenderTargetSkia::MaybeClearCurrent() const {
  if (on_clear_current_ != nullptr) {
    return on_clear_current_();
  }

  return {true, false};
}

}  // namespace appcode
