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

#ifndef APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_RENDER_TARGET_SKIA_H_
#define APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_RENDER_TARGET_SKIA_H_

#include "appcode/shell/platform/embedder/embedder_render_target.h"

namespace appcode {

class EmbedderRenderTargetSkia final : public EmbedderRenderTarget {
 public:
  EmbedderRenderTargetSkia(FlutterBackingStore backing_store,
                           sk_sp<SkSurface> render_surface,
                           fml::closure on_release,
                           MakeOrClearCurrentCallback on_make_current,
                           MakeOrClearCurrentCallback on_clear_current);

  // |EmbedderRenderTarget|
  ~EmbedderRenderTargetSkia() override;

  // |EmbedderRenderTarget|
  sk_sp<SkSurface> GetSkiaSurface() const override;

  // |EmbedderRenderTarget|
  impeller::RenderTarget* GetImpellerRenderTarget() const override;

  // |EmbedderRenderTarget|
  std::shared_ptr<impeller::AiksContext> GetAiksContext() const override;

  // |EmbedderRenderTarget|
  SkISize GetRenderTargetSize() const override;

  // |EmbedderRenderTarget|
  SetCurrentResult MaybeMakeCurrent() const override;

  // |EmbedderRenderTarget|
  SetCurrentResult MaybeClearCurrent() const override;

 private:
  sk_sp<SkSurface> render_surface_;

  MakeOrClearCurrentCallback on_make_current_;
  MakeOrClearCurrentCallback on_clear_current_;

  FML_DISALLOW_COPY_AND_ASSIGN(EmbedderRenderTargetSkia);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_RENDER_TARGET_SKIA_H_
