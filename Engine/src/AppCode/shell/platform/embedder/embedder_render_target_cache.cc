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

#include "appcode/shell/platform/embedder/embedder_render_target_cache.h"

namespace appcode {

EmbedderRenderTargetCache::EmbedderRenderTargetCache() = default;

EmbedderRenderTargetCache::~EmbedderRenderTargetCache() = default;

std::unique_ptr<EmbedderRenderTarget>
EmbedderRenderTargetCache::GetRenderTarget(
    const EmbedderExternalView::RenderTargetDescriptor& descriptor) {
  auto compatible_target = cached_render_targets_.find(descriptor);
  if (compatible_target == cached_render_targets_.end()) {
    return nullptr;
  }
  auto target = std::move(compatible_target->second);
  cached_render_targets_.erase(compatible_target);
  return target;
}

std::set<std::unique_ptr<EmbedderRenderTarget>>
EmbedderRenderTargetCache::ClearAllRenderTargetsInCache() {
  std::set<std::unique_ptr<EmbedderRenderTarget>> cleared_targets;
  for (auto& targets : cached_render_targets_) {
    cleared_targets.insert(std::move(targets.second));
  }
  cached_render_targets_.clear();
  return cleared_targets;
}

void EmbedderRenderTargetCache::CacheRenderTarget(
    std::unique_ptr<EmbedderRenderTarget> target) {
  if (target == nullptr) {
    return;
  }
  auto desc = EmbedderExternalView::RenderTargetDescriptor{
      target->GetRenderTargetSize()};
  cached_render_targets_.insert(std::make_pair(desc, std::move(target)));
}

size_t EmbedderRenderTargetCache::GetCachedTargetsCount() const {
  return cached_render_targets_.size();
}

}  // namespace appcode
