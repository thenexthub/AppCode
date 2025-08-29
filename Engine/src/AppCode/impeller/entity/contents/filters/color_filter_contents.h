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

#ifndef APPCODE_IMPELLER_ENTITY_CONTENTS_FILTERS_COLOR_FILTER_CONTENTS_H_
#define APPCODE_IMPELLER_ENTITY_CONTENTS_FILTERS_COLOR_FILTER_CONTENTS_H_

#include "impeller/entity/contents/filters/filter_contents.h"

namespace impeller {

class ColorFilterContents : public FilterContents {
 public:
  enum class AbsorbOpacity {
    kYes,
    kNo,
  };

  /// @brief the [inputs] are expected to be in the order of dst, src.
  static std::shared_ptr<ColorFilterContents> MakeBlend(
      BlendMode blend_mode,
      FilterInput::Vector inputs,
      std::optional<Color> foreground_color = std::nullopt);

  static std::shared_ptr<ColorFilterContents> MakeColorMatrix(
      FilterInput::Ref input,
      const ColorMatrix& color_matrix);

  static std::shared_ptr<ColorFilterContents> MakeLinearToSrgbFilter(
      FilterInput::Ref input);

  static std::shared_ptr<ColorFilterContents> MakeSrgbToLinearFilter(
      FilterInput::Ref input);

  ColorFilterContents();

  ~ColorFilterContents() override;

  void SetAbsorbOpacity(AbsorbOpacity absorb_opacity);

  AbsorbOpacity GetAbsorbOpacity() const;

  /// @brief Sets an alpha that is applied to the final blended result.
  void SetAlpha(Scalar alpha);

  std::optional<Scalar> GetAlpha() const;

  // |Contents|
  void SetInheritedOpacity(Scalar opacity) override;

  // |FilterContents|
  std::optional<Rect> GetFilterSourceCoverage(
      const Matrix& effect_transform,
      const Rect& output_limit) const override;

 private:
  AbsorbOpacity absorb_opacity_ = AbsorbOpacity::kNo;
  std::optional<Scalar> alpha_;
  Scalar inherited_opacity_ = 1.0;

  ColorFilterContents(const ColorFilterContents&) = delete;

  ColorFilterContents& operator=(const ColorFilterContents&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_CONTENTS_FILTERS_COLOR_FILTER_CONTENTS_H_
