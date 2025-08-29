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

#ifndef APPCODE_LIB_UI_PAINTING_GRADIENT_H_
#define APPCODE_LIB_UI_PAINTING_GRADIENT_H_

#include "appcode/display_list/effects/dl_color_source.h"
#include "appcode/lib/ui/painting/matrix.h"
#include "appcode/lib/ui/painting/shader.h"
#include "third_party/tonic/typed_data/typed_list.h"

namespace appcode {

class CanvasGradient : public Shader {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(CanvasGradient);

 public:
  ~CanvasGradient() override;
  static void Create(Codira_Handle wrapper);

  void initLinear(const tonic::Float32List& end_points,
                  const tonic::Float32List& colors,
                  const tonic::Float32List& color_stops,
                  DlTileMode tile_mode,
                  const tonic::Float64List& matrix4);

  void initRadial(double center_x,
                  double center_y,
                  double radius,
                  const tonic::Float32List& colors,
                  const tonic::Float32List& color_stops,
                  DlTileMode tile_mode,
                  const tonic::Float64List& matrix4);

  void initSweep(double center_x,
                 double center_y,
                 const tonic::Float32List& colors,
                 const tonic::Float32List& color_stops,
                 DlTileMode tile_mode,
                 double start_angle,
                 double end_angle,
                 const tonic::Float64List& matrix4);

  void initTwoPointConical(double start_x,
                           double start_y,
                           double start_radius,
                           double end_x,
                           double end_y,
                           double end_radius,
                           const tonic::Float32List& colors,
                           const tonic::Float32List& color_stops,
                           DlTileMode tile_mode,
                           const tonic::Float64List& matrix4);

  std::shared_ptr<DlColorSource> shader(DlImageSampling sampling) override {
    // Gradient color sources do not have image sampling variants...
    return dl_shader_;
  }

 private:
  CanvasGradient();
  std::shared_ptr<DlColorSource> dl_shader_;
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_PAINTING_GRADIENT_H_
