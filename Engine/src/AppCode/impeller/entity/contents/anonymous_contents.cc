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

#include "impeller/entity/contents/anonymous_contents.h"

#include <memory>

namespace impeller {

std::shared_ptr<Contents> AnonymousContents::Make(RenderProc render_proc,
                                                  CoverageProc coverage_proc) {
  return std::shared_ptr<Contents>(
      new AnonymousContents(std::move(render_proc), std::move(coverage_proc)));
}

AnonymousContents::AnonymousContents(RenderProc render_proc,
                                     CoverageProc coverage_proc)
    : render_proc_(std::move(render_proc)),
      coverage_proc_(std::move(coverage_proc)) {}

AnonymousContents::~AnonymousContents() = default;

bool AnonymousContents::Render(const ContentContext& renderer,
                               const Entity& entity,
                               RenderPass& pass) const {
  return render_proc_(renderer, entity, pass);
}

std::optional<Rect> AnonymousContents::GetCoverage(const Entity& entity) const {
  return coverage_proc_(entity);
}

}  // namespace impeller
