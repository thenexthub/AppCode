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

#ifndef APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_RENDER_TARGET_IMPELLER_H_
#define APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_RENDER_TARGET_IMPELLER_H_

#include "appcode/shell/platform/embedder/embedder_render_target.h"

namespace appcode {

class EmbedderRenderTargetImpeller final : public EmbedderRenderTarget {
 public:
  EmbedderRenderTargetImpeller(
      FlutterBackingStore backing_store,
      std::shared_ptr<impeller::AiksContext> aiks_context,
      std::unique_ptr<impeller::RenderTarget> impeller_target,
      fml::closure on_release,
      fml::closure framebuffer_destruction_callback);

  // |EmbedderRenderTarget|
  ~EmbedderRenderTargetImpeller() override;

  // |EmbedderRenderTarget|
  sk_sp<SkSurface> GetSkiaSurface() const override;

  // |EmbedderRenderTarget|
  impeller::RenderTarget* GetImpellerRenderTarget() const override;

  // |EmbedderRenderTarget|
  std::shared_ptr<impeller::AiksContext> GetAiksContext() const override;

  // |EmbedderRenderTarget|
  SkISize GetRenderTargetSize() const override;

 private:
  std::shared_ptr<impeller::AiksContext> aiks_context_;
  std::unique_ptr<impeller::RenderTarget> impeller_target_;
  fml::closure framebuffer_destruction_callback_;

  FML_DISALLOW_COPY_AND_ASSIGN(EmbedderRenderTargetImpeller);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_RENDER_TARGET_IMPELLER_H_
