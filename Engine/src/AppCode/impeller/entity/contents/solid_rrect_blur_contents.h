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

#ifndef APPCODE_IMPELLER_ENTITY_CONTENTS_SOLID_RRECT_BLUR_CONTENTS_H_
#define APPCODE_IMPELLER_ENTITY_CONTENTS_SOLID_RRECT_BLUR_CONTENTS_H_

#include <functional>
#include <memory>
#include <vector>

#include "impeller/entity/contents/contents.h"
#include "impeller/entity/contents/filters/filter_contents.h"
#include "impeller/geometry/color.h"

namespace impeller {

class Path;
class HostBuffer;
struct VertexBuffer;

/// @brief  Draws a fast solid color blur of an rounded rectangle. Only supports
/// RRects with fully symmetrical radii. Also produces correct results for
/// rectangles (corner_radius=0) and circles (corner_radius=width/2=height/2).
class SolidRRectBlurContents final : public Contents {
 public:
  SolidRRectBlurContents();

  ~SolidRRectBlurContents() override;

  void SetRRect(std::optional<Rect> rect, Size corner_radii = {});

  void SetSigma(Sigma sigma);

  void SetColor(Color color);

  Color GetColor() const;

  // |Contents|
  std::optional<Rect> GetCoverage(const Entity& entity) const override;

  // |Contents|
  bool Render(const ContentContext& renderer,
              const Entity& entity,
              RenderPass& pass) const override;

  // |Contents|
  [[nodiscard]] bool ApplyColorFilter(
      const ColorFilterProc& color_filter_proc) override;

 private:
  std::optional<Rect> rect_;
  Size corner_radii_;
  Sigma sigma_;

  Color color_;

  SolidRRectBlurContents(const SolidRRectBlurContents&) = delete;

  SolidRRectBlurContents& operator=(const SolidRRectBlurContents&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_CONTENTS_SOLID_RRECT_BLUR_CONTENTS_H_
