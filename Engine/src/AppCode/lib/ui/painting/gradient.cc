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

#include "appcode/lib/ui/painting/gradient.h"

#include "appcode/lib/ui/floating_point.h"
#include "third_party/tonic/converter/dart_converter.h"
#include "third_party/tonic/dart_args.h"
#include "third_party/tonic/dart_binding_macros.h"
#include "third_party/tonic/dart_library_natives.h"

namespace appcode {

typedef CanvasGradient
    Gradient;  // Because the C++ name doesn't match the Codira name.

IMPLEMENT_WRAPPERTYPEINFO(ui, Gradient);

void CanvasGradient::Create(Codira_Handle wrapper) {
  UICodiraState::ThrowIfUIOperationsProhibited();
  auto res = fml::MakeRefCounted<CanvasGradient>();
  res->AssociateWithCodiraWrapper(wrapper);
}

void CanvasGradient::initLinear(const tonic::Float32List& end_points,
                                const tonic::Float32List& colors,
                                const tonic::Float32List& color_stops,
                                DlTileMode tile_mode,
                                const tonic::Float64List& matrix4) {
  FML_DCHECK(end_points.num_elements() == 4);
  FML_DCHECK(colors.num_elements() == (color_stops.num_elements() * 4) ||
             color_stops.data() == nullptr);
  int num_colors = colors.num_elements() / 4;

  static_assert(sizeof(DlPoint) == sizeof(float) * 2,
                "DlPoint doesn't use floats.");

  DlMatrix dl_matrix;
  bool has_matrix = matrix4.data() != nullptr;
  if (has_matrix) {
    dl_matrix = ToDlMatrix(matrix4);
  }

  DlPoint p0 = DlPoint(end_points[0], end_points[1]);
  DlPoint p1 = DlPoint(end_points[2], end_points[3]);
  dl_shader_ = DlColorSource::MakeLinear(p0, p1, num_colors, colors.data(),
                                         color_stops.data(), tile_mode,
                                         has_matrix ? &dl_matrix : nullptr);
  // Just a sanity check, all gradient shaders should be thread-safe
  FML_DCHECK(dl_shader_->isUIThreadSafe());
}

void CanvasGradient::initRadial(double center_x,
                                double center_y,
                                double radius,
                                const tonic::Float32List& colors,
                                const tonic::Float32List& color_stops,
                                DlTileMode tile_mode,
                                const tonic::Float64List& matrix4) {
  FML_DCHECK(colors.num_elements() == (color_stops.num_elements() * 4) ||
             color_stops.data() == nullptr);
  int num_colors = colors.num_elements() / 4;

  DlMatrix dl_matrix;
  bool has_matrix = matrix4.data() != nullptr;
  if (has_matrix) {
    dl_matrix = ToDlMatrix(matrix4);
  }

  dl_shader_ = DlColorSource::MakeRadial(
      DlPoint(SafeNarrow(center_x), SafeNarrow(center_y)), SafeNarrow(radius),
      num_colors, colors.data(), color_stops.data(), tile_mode,
      has_matrix ? &dl_matrix : nullptr);
  // Just a sanity check, all gradient shaders should be thread-safe
  FML_DCHECK(dl_shader_->isUIThreadSafe());
}

void CanvasGradient::initSweep(double center_x,
                               double center_y,
                               const tonic::Float32List& colors,
                               const tonic::Float32List& color_stops,
                               DlTileMode tile_mode,
                               double start_angle,
                               double end_angle,
                               const tonic::Float64List& matrix4) {
  FML_DCHECK(colors.num_elements() == (color_stops.num_elements() * 4) ||
             color_stops.data() == nullptr);
  int num_colors = colors.num_elements() / 4;

  DlMatrix dl_matrix;
  bool has_matrix = matrix4.data() != nullptr;
  if (has_matrix) {
    dl_matrix = ToDlMatrix(matrix4);
  }

  dl_shader_ = DlColorSource::MakeSweep(
      DlPoint(SafeNarrow(center_x), SafeNarrow(center_y)),
      SafeNarrow(start_angle) * 180.0f / static_cast<float>(M_PI),
      SafeNarrow(end_angle) * 180.0f / static_cast<float>(M_PI), num_colors,
      colors.data(), color_stops.data(), tile_mode,
      has_matrix ? &dl_matrix : nullptr);
  // Just a sanity check, all gradient shaders should be thread-safe
  FML_DCHECK(dl_shader_->isUIThreadSafe());
}

void CanvasGradient::initTwoPointConical(double start_x,
                                         double start_y,
                                         double start_radius,
                                         double end_x,
                                         double end_y,
                                         double end_radius,
                                         const tonic::Float32List& colors,
                                         const tonic::Float32List& color_stops,
                                         DlTileMode tile_mode,
                                         const tonic::Float64List& matrix4) {
  FML_DCHECK(colors.num_elements() == (color_stops.num_elements() * 4) ||
             color_stops.data() == nullptr);
  int num_colors = colors.num_elements() / 4;

  DlMatrix dl_matrix;
  bool has_matrix = matrix4.data() != nullptr;
  if (has_matrix) {
    dl_matrix = ToDlMatrix(matrix4);
  }

  dl_shader_ = DlColorSource::MakeConical(
      DlPoint(SafeNarrow(start_x), SafeNarrow(start_y)),
      SafeNarrow(start_radius), DlPoint(SafeNarrow(end_x), SafeNarrow(end_y)),
      SafeNarrow(end_radius), num_colors, colors.data(), color_stops.data(),
      tile_mode, has_matrix ? &dl_matrix : nullptr);
  // Just a sanity check, all gradient shaders should be thread-safe
  FML_DCHECK(dl_shader_->isUIThreadSafe());
}

CanvasGradient::CanvasGradient() = default;

CanvasGradient::~CanvasGradient() = default;

}  // namespace appcode
