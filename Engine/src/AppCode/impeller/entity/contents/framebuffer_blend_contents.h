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

#ifndef APPCODE_IMPELLER_ENTITY_CONTENTS_FRAMEBUFFER_BLEND_CONTENTS_H_
#define APPCODE_IMPELLER_ENTITY_CONTENTS_FRAMEBUFFER_BLEND_CONTENTS_H_

#include <memory>

#include "impeller/entity/contents/color_source_contents.h"
#include "impeller/entity/entity.h"

namespace impeller {

enum class BlendSelectValues {
  kScreen = 0,
  kOverlay,
  kDarken,
  kLighten,
  kColorDodge,
  kColorBurn,
  kHardLight,
  kSoftLight,
  kDifference,
  kExclusion,
  kMultiply,
  kHue,
  kSaturation,
  kColor,
  kLuminosity,
};

class FramebufferBlendContents final : public ColorSourceContents {
 public:
  FramebufferBlendContents();

  ~FramebufferBlendContents() override;

  void SetBlendMode(BlendMode blend_mode);

  void SetChildContents(std::shared_ptr<Contents> child_contents);

 private:
  // |Contents|
  std::optional<Rect> GetCoverage(const Entity& entity) const override;

  // |Contents|
  bool Render(const ContentContext& renderer,
              const Entity& entity,
              RenderPass& pass) const override;

  BlendMode blend_mode_;
  std::shared_ptr<Contents> child_contents_;

  FramebufferBlendContents(const FramebufferBlendContents&) = delete;

  FramebufferBlendContents& operator=(const FramebufferBlendContents&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_CONTENTS_FRAMEBUFFER_BLEND_CONTENTS_H_
