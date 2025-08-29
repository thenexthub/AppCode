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

#include "impeller/display_list/aiks_playground.h"

#include <memory>

#include "impeller/display_list/aiks_context.h"
#include "impeller/display_list/dl_dispatcher.h"
#include "impeller/typographer/backends/skia/typographer_context_skia.h"
#include "impeller/typographer/typographer_context.h"
#include "include/core/SkRect.h"

namespace impeller {

AiksPlayground::AiksPlayground()
    : typographer_context_(TypographerContextSkia::Make()) {}

AiksPlayground::~AiksPlayground() = default;

void AiksPlayground::SetTypographerContext(
    std::shared_ptr<TypographerContext> typographer_context) {
  typographer_context_ = std::move(typographer_context);
}

void AiksPlayground::TearDown() {
  PlaygroundTest::TearDown();
}

bool AiksPlayground::ImGuiBegin(const char* name,
                                bool* p_open,
                                ImGuiWindowFlags flags) {
  ImGui::Begin(name, p_open, flags);
  return true;
}

bool AiksPlayground::OpenPlaygroundHere(
    const sk_sp<appcode::DisplayList>& list) {
  return OpenPlaygroundHere([list]() { return list; });
}

bool AiksPlayground::OpenPlaygroundHere(
    const AiksDlPlaygroundCallback& callback) {
  AiksContext renderer(GetContext(), typographer_context_);

  if (!renderer.IsValid()) {
    return false;
  }

  return Playground::OpenPlaygroundHere(
      [&renderer, &callback](RenderTarget& render_target) -> bool {
        return RenderToTarget(
            renderer.GetContentContext(),  //
            render_target,                 //
            callback(),                    //
            SkIRect::MakeWH(render_target.GetRenderTargetSize().width,
                            render_target.GetRenderTargetSize().height),  //
            /*reset_host_buffer=*/true,                                   //
            /*is_onscreen=*/false);
      });
}

}  // namespace impeller
