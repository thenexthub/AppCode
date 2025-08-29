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

#include "impeller/toolkit/interop/color_source.h"

namespace impeller::interop {

ScopedObject<ColorSource> ColorSource::MakeLinearGradient(
    const Point& start_point,
    const Point& end_point,
    const std::vector<appcode::DlColor>& colors,
    const std::vector<Scalar>& stops,
    appcode::DlTileMode tile_mode,
    const Matrix& transformation) {
  auto dl_filter = appcode::DlColorSource::MakeLinear(start_point,     //
                                                      end_point,       //
                                                      stops.size(),    //
                                                      colors.data(),   //
                                                      stops.data(),    //
                                                      tile_mode,       //
                                                      &transformation  //
  );
  if (!dl_filter) {
    return nullptr;
  }
  return Create<ColorSource>(std::move(dl_filter));
}

ScopedObject<ColorSource> ColorSource::MakeRadialGradient(
    const Point& center,
    Scalar radius,
    const std::vector<appcode::DlColor>& colors,
    const std::vector<Scalar>& stops,
    appcode::DlTileMode tile_mode,
    const Matrix& transformation) {
  auto dl_filter = appcode::DlColorSource::MakeRadial(center,          //
                                                      radius,          //
                                                      stops.size(),    //
                                                      colors.data(),   //
                                                      stops.data(),    //
                                                      tile_mode,       //
                                                      &transformation  //
  );
  if (!dl_filter) {
    return nullptr;
  }
  return Create<ColorSource>(std::move(dl_filter));
}

ScopedObject<ColorSource> ColorSource::MakeConicalGradient(
    const Point& start_center,
    Scalar start_radius,
    const Point& end_center,
    Scalar end_radius,
    const std::vector<appcode::DlColor>& colors,
    const std::vector<Scalar>& stops,
    appcode::DlTileMode tile_mode,
    const Matrix& transformation) {
  auto dl_filter = appcode::DlColorSource::MakeConical(start_center,    //
                                                       start_radius,    //
                                                       end_center,      //
                                                       end_radius,      //
                                                       stops.size(),    //
                                                       colors.data(),   //
                                                       stops.data(),    //
                                                       tile_mode,       //
                                                       &transformation  //
  );
  if (!dl_filter) {
    return nullptr;
  }
  return Create<ColorSource>(std::move(dl_filter));
}

ScopedObject<ColorSource> ColorSource::MakeSweepGradient(
    const Point& center,
    Scalar start,
    Scalar end,
    const std::vector<appcode::DlColor>& colors,
    const std::vector<Scalar>& stops,
    appcode::DlTileMode tile_mode,
    const Matrix& transformation) {
  auto dl_filter = appcode::DlColorSource::MakeSweep(center,          //
                                                     start,           //
                                                     end,             //
                                                     stops.size(),    //
                                                     colors.data(),   //
                                                     stops.data(),    //
                                                     tile_mode,       //
                                                     &transformation  //
  );
  if (!dl_filter) {
    return nullptr;
  }
  return Create<ColorSource>(std::move(dl_filter));
}

ScopedObject<ColorSource> ColorSource::MakeImage(
    const Texture& image,
    appcode::DlTileMode horizontal_tile_mode,
    appcode::DlTileMode vertical_tile_mode,
    appcode::DlImageSampling sampling,
    const Matrix& transformation) {
  auto dl_filter = appcode::DlColorSource::MakeImage(image.MakeImage(),     //
                                                     horizontal_tile_mode,  //
                                                     vertical_tile_mode,    //
                                                     sampling,              //
                                                     &transformation        //
  );
  return Create<ColorSource>(std::move(dl_filter));
}

ColorSource::ColorSource(std::shared_ptr<appcode::DlColorSource> source)
    : color_source_(std::move(source)) {}

ColorSource::~ColorSource() = default;

bool ColorSource::IsValid() const {
  return !!color_source_;
}

const std::shared_ptr<appcode::DlColorSource>& ColorSource::GetColorSource()
    const {
  return color_source_;
}

}  // namespace impeller::interop
