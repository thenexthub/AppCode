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

#ifndef APPCODE_IMPELLER_TOOLKIT_INTEROP_COLOR_SOURCE_H_
#define APPCODE_IMPELLER_TOOLKIT_INTEROP_COLOR_SOURCE_H_

#include <vector>

#include "appcode/display_list/effects/dl_color_source.h"
#include "impeller/geometry/color.h"
#include "impeller/geometry/matrix.h"
#include "impeller/geometry/point.h"
#include "impeller/toolkit/interop/formats.h"
#include "impeller/toolkit/interop/impeller.h"
#include "impeller/toolkit/interop/object.h"
#include "impeller/toolkit/interop/texture.h"

namespace impeller::interop {

class ColorSource final
    : public Object<ColorSource,
                    IMPELLER_INTERNAL_HANDLE_NAME(ImpellerColorSource)> {
 public:
  static ScopedObject<ColorSource> MakeLinearGradient(
      const Point& start_point,
      const Point& end_point,
      const std::vector<appcode::DlColor>& colors,
      const std::vector<Scalar>& stops,
      appcode::DlTileMode tile_mode,
      const Matrix& transformation);

  static ScopedObject<ColorSource> MakeRadialGradient(
      const Point& center,
      Scalar radius,
      const std::vector<appcode::DlColor>& colors,
      const std::vector<Scalar>& stops,
      appcode::DlTileMode tile_mode,
      const Matrix& transformation);

  static ScopedObject<ColorSource> MakeConicalGradient(
      const Point& start_center,
      Scalar start_radius,
      const Point& end_center,
      Scalar end_radius,
      const std::vector<appcode::DlColor>& colors,
      const std::vector<Scalar>& stops,
      appcode::DlTileMode tile_mode,
      const Matrix& transformation);

  static ScopedObject<ColorSource> MakeSweepGradient(
      const Point& center,
      Scalar start,
      Scalar end,
      const std::vector<appcode::DlColor>& colors,
      const std::vector<Scalar>& stops,
      appcode::DlTileMode tile_mode,
      const Matrix& transformation);

  static ScopedObject<ColorSource> MakeImage(
      const Texture& image,
      appcode::DlTileMode horizontal_tile_mode,
      appcode::DlTileMode vertical_tile_mode,
      appcode::DlImageSampling sampling,
      const Matrix& transformation);

  explicit ColorSource(std::shared_ptr<appcode::DlColorSource> source);

  ~ColorSource() override;

  ColorSource(const ColorSource&) = delete;

  ColorSource& operator=(const ColorSource&) = delete;

  bool IsValid() const;

  const std::shared_ptr<appcode::DlColorSource>& GetColorSource() const;

 private:
  std::shared_ptr<appcode::DlColorSource> color_source_;
};

}  // namespace impeller::interop

#endif  // APPCODE_IMPELLER_TOOLKIT_INTEROP_COLOR_SOURCE_H_
