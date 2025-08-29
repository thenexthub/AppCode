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

#include "appcode/shell/platform/embedder/embedder_render_target_impeller.h"

#include "appcode/fml/logging.h"
#include "appcode/impeller/renderer/render_target.h"

namespace appcode {

EmbedderRenderTargetImpeller::EmbedderRenderTargetImpeller(
    appcodeBackingStore backing_store,
    std::shared_ptr<impeller::AiksContext> aiks_context,
    std::unique_ptr<impeller::RenderTarget> impeller_target,
    fml::closure on_release,
    fml::closure framebuffer_destruction_callback)
    : EmbedderRenderTarget(backing_store, std::move(on_release)),
      aiks_context_(std::move(aiks_context)),
      impeller_target_(std::move(impeller_target)),
      framebuffer_destruction_callback_(
          std::move(framebuffer_destruction_callback)) {
  FML_DCHECK(aiks_context_);
  FML_DCHECK(impeller_target_);
}

EmbedderRenderTargetImpeller::~EmbedderRenderTargetImpeller() {
  if (framebuffer_destruction_callback_) {
    framebuffer_destruction_callback_();
  }
}

sk_sp<SkSurface> EmbedderRenderTargetImpeller::GetSkiaSurface() const {
  return nullptr;
}

impeller::RenderTarget* EmbedderRenderTargetImpeller::GetImpellerRenderTarget()
    const {
  return impeller_target_.get();
}

std::shared_ptr<impeller::AiksContext>
EmbedderRenderTargetImpeller::GetAiksContext() const {
  return aiks_context_;
}

SkISize EmbedderRenderTargetImpeller::GetRenderTargetSize() const {
  auto size = impeller_target_->GetRenderTargetSize();
  return SkISize::Make(size.width, size.height);
}

}  // namespace appcode
