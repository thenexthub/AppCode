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

#ifndef APPCODE_IMPELLER_ENTITY_ENTITY_PASS_TARGET_H_
#define APPCODE_IMPELLER_ENTITY_ENTITY_PASS_TARGET_H_

#include "impeller/entity/contents/content_context.h"
#include "impeller/renderer/render_target.h"

namespace impeller {

class InlinePassContext;

class EntityPassTarget {
 public:
  explicit EntityPassTarget(const RenderTarget& render_target,
                            bool supports_read_from_resolve,
                            bool supports_implicit_msaa);

  /// @brief  Flips the backdrop and returns a readable texture that can be
  ///         bound/sampled to restore the previous pass.
  ///
  ///         After this method is called, a new `RenderPass` that attaches the
  ///         result of `GetRenderTarget` is guaranteed to be able to read the
  ///         previous pass's backdrop texture (which is returned by this
  ///         method).
  std::shared_ptr<Texture> Flip(const ContentContext& renderer);

  RenderTarget& GetRenderTarget();

  /// @brief Remove the cached secondary color texture.
  void RemoveSecondary();

  bool IsValid() const;

 private:
  RenderTarget target_;
  std::shared_ptr<Texture> secondary_color_texture_;

  bool supports_read_from_resolve_;
  bool supports_implicit_msaa_;

  friend InlinePassContext;

  EntityPassTarget& operator=(const EntityPassTarget&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_ENTITY_PASS_TARGET_H_
