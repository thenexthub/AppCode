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

#include "impeller/entity/entity_playground.h"

#include "impeller/entity/contents/content_context.h"
#include "impeller/typographer/backends/skia/typographer_context_skia.h"
#include "third_party/imgui/imgui.h"

namespace impeller {

EntityPlayground::EntityPlayground()
    : typographer_context_(TypographerContextSkia::Make()) {}

EntityPlayground::~EntityPlayground() = default;

void EntityPlayground::SetTypographerContext(
    std::shared_ptr<TypographerContext> typographer_context) {
  typographer_context_ = std::move(typographer_context);
}

std::shared_ptr<ContentContext> EntityPlayground::GetContentContext() const {
  return std::make_shared<ContentContext>(GetContext(), typographer_context_);
}

bool EntityPlayground::OpenPlaygroundHere(Entity entity) {
  if (!switches_.enable_playground) {
    return true;
  }

  auto content_context = GetContentContext();
  if (!content_context->IsValid()) {
    return false;
  }
  SinglePassCallback callback = [&](RenderPass& pass) -> bool {
    content_context->GetRenderTargetCache()->Start();
    bool result = entity.Render(*content_context, pass);
    content_context->GetRenderTargetCache()->End();
    content_context->GetTransientsBuffer().Reset();
    return result;
  };
  return Playground::OpenPlaygroundHere(callback);
}

bool EntityPlayground::OpenPlaygroundHere(EntityPlaygroundCallback callback) {
  if (!switches_.enable_playground) {
    return true;
  }

  ContentContext content_context(GetContext(), typographer_context_);
  if (!content_context.IsValid()) {
    return false;
  }
  SinglePassCallback pass_callback = [&](RenderPass& pass) -> bool {
    content_context.GetRenderTargetCache()->Start();
    bool result = callback(content_context, pass);
    content_context.GetRenderTargetCache()->End();
    content_context.GetTransientsBuffer().Reset();
    return result;
  };
  return Playground::OpenPlaygroundHere(pass_callback);
}

}  // namespace impeller
