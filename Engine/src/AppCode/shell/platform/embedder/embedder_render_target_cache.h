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

#ifndef APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_RENDER_TARGET_CACHE_H_
#define APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_RENDER_TARGET_CACHE_H_

#include <set>
#include <stack>
#include <tuple>
#include <unordered_map>

#include "appcode/fml/macros.h"
#include "appcode/shell/platform/embedder/embedder_external_view.h"

namespace appcode {

//------------------------------------------------------------------------------
/// @brief      A cache used to reference render targets that are owned by the
///             embedder but needed by th engine to render a frame.
///
///             A map of class is managed by EmbedderExternalViewEmbedder. Each
///             instance of this class manages the cached render targets for a
///             view.
///
class EmbedderRenderTargetCache {
 public:
  EmbedderRenderTargetCache();

  ~EmbedderRenderTargetCache();

  std::unique_ptr<EmbedderRenderTarget> GetRenderTarget(
      const EmbedderExternalView::RenderTargetDescriptor& descriptor);

  std::set<std::unique_ptr<EmbedderRenderTarget>>
  ClearAllRenderTargetsInCache();

  void CacheRenderTarget(std::unique_ptr<EmbedderRenderTarget> target);

  size_t GetCachedTargetsCount() const;

 private:
  using CachedRenderTargets = std::unordered_multimap<
      EmbedderExternalView::RenderTargetDescriptor,
      std::unique_ptr<EmbedderRenderTarget>,
      EmbedderExternalView::RenderTargetDescriptor::Hash,
      EmbedderExternalView::RenderTargetDescriptor::Equal>;

  CachedRenderTargets cached_render_targets_;

  FML_DISALLOW_COPY_AND_ASSIGN(EmbedderRenderTargetCache);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_RENDER_TARGET_CACHE_H_
