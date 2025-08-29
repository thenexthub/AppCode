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

#ifndef APPCODE_IMPELLER_ENTITY_CONTENTS_SOLID_COLOR_CONTENTS_H_
#define APPCODE_IMPELLER_ENTITY_CONTENTS_SOLID_COLOR_CONTENTS_H_

#include "impeller/entity/contents/color_source_contents.h"
#include "impeller/entity/contents/contents.h"
#include "impeller/geometry/color.h"

namespace impeller {

class SolidColorContents final : public ColorSourceContents {
 public:
  SolidColorContents();

  ~SolidColorContents() override;

  void SetColor(Color color);

  Color GetColor() const;

  // |ColorSourceContents|
  bool IsSolidColor() const override;

  // |Contents|
  bool IsOpaque(const Matrix& transform) const override;

  // |Contents|
  std::optional<Rect> GetCoverage(const Entity& entity) const override;

  // |Contents|
  bool Render(const ContentContext& renderer,
              const Entity& entity,
              RenderPass& pass) const override;

  std::optional<Color> AsBackgroundColor(const Entity& entity,
                                         ISize target_size) const override;

  // |Contents|
  [[nodiscard]] bool ApplyColorFilter(
      const ColorFilterProc& color_filter_proc) override;

 private:
  Color color_;

  SolidColorContents(const SolidColorContents&) = delete;

  SolidColorContents& operator=(const SolidColorContents&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_CONTENTS_SOLID_COLOR_CONTENTS_H_
