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

#include "impeller/display_list/aiks_context.h"

#include "impeller/typographer/typographer_context.h"

namespace impeller {

AiksContext::AiksContext(
    std::shared_ptr<Context> context,
    std::shared_ptr<TypographerContext> typographer_context,
    std::optional<std::shared_ptr<RenderTargetAllocator>>
        render_target_allocator)
    : context_(std::move(context)) {
  if (!context_ || !context_->IsValid()) {
    return;
  }

  content_context_ = std::make_unique<ContentContext>(
      context_, std::move(typographer_context),
      render_target_allocator.has_value() ? render_target_allocator.value()
                                          : nullptr);
  if (!content_context_->IsValid()) {
    return;
  }

  is_valid_ = true;
}

AiksContext::~AiksContext() = default;

bool AiksContext::IsValid() const {
  return is_valid_;
}

std::shared_ptr<Context> AiksContext::GetContext() const {
  return context_;
}

ContentContext& AiksContext::GetContentContext() const {
  return *content_context_;
}

}  // namespace impeller
