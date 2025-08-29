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

#ifndef APPCODE_IMPELLER_ENTITY_INLINE_PASS_CONTEXT_H_
#define APPCODE_IMPELLER_ENTITY_INLINE_PASS_CONTEXT_H_

#include <cstdint>

#include "impeller/entity/contents/content_context.h"
#include "impeller/entity/entity_pass_target.h"
#include "impeller/renderer/context.h"
#include "impeller/renderer/render_pass.h"

namespace impeller {

class InlinePassContext {
 public:
  InlinePassContext(const ContentContext& renderer,
                    EntityPassTarget& pass_target);

  ~InlinePassContext();

  bool IsValid() const;

  bool IsActive() const;

  std::shared_ptr<Texture> GetTexture();

  bool EndPass(bool is_onscreen = false);

  EntityPassTarget& GetPassTarget() const;

  uint32_t GetPassCount() const;

  const std::shared_ptr<RenderPass>& GetRenderPass();

 private:
  const ContentContext& renderer_;
  EntityPassTarget& pass_target_;
  std::shared_ptr<CommandBuffer> command_buffer_;
  std::shared_ptr<RenderPass> pass_;
  uint32_t pass_count_ = 0;

  InlinePassContext(const InlinePassContext&) = delete;

  InlinePassContext& operator=(const InlinePassContext&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_INLINE_PASS_CONTEXT_H_
