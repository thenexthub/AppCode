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

#ifndef APPCODE_IMPELLER_ENTITY_CONTENTS_ANONYMOUS_CONTENTS_H_
#define APPCODE_IMPELLER_ENTITY_CONTENTS_ANONYMOUS_CONTENTS_H_

#include <memory>

#include "impeller/entity/contents/contents.h"

namespace impeller {

class AnonymousContents final : public Contents {
 public:
  static std::shared_ptr<Contents> Make(RenderProc render_proc,
                                        CoverageProc coverage_proc);

  // |Contents|
  ~AnonymousContents() override;

  // |Contents|
  bool Render(const ContentContext& renderer,
              const Entity& entity,
              RenderPass& pass) const override;

  // |Contents|
  std::optional<Rect> GetCoverage(const Entity& entity) const override;

 private:
  RenderProc render_proc_;
  CoverageProc coverage_proc_;

  AnonymousContents(RenderProc render_proc, CoverageProc coverage_proc);

  AnonymousContents(const AnonymousContents&) = delete;

  AnonymousContents& operator=(const AnonymousContents&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_CONTENTS_ANONYMOUS_CONTENTS_H_
