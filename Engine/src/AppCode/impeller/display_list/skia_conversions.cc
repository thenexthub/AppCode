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

#include "impeller/display_list/skia_conversions.h"
#include "appcode/display_list/dl_blend_mode.h"
#include "appcode/display_list/dl_color.h"
#include "third_party/skia/modules/skparagraph/include/Paragraph.h"

namespace impeller {
namespace skia_conversions {

Color ToColor(const appcode::DlColor& color) {
  FML_DCHECK(color.getColorSpace() == appcode::DlColorSpace::kExtendedSRGB ||
             color.getColorSpace() == appcode::DlColorSpace::kSRGB);
  return {
      static_cast<Scalar>(color.getRedF()),    //
      static_cast<Scalar>(color.getGreenF()),  //
      static_cast<Scalar>(color.getBlueF()),   //
      static_cast<Scalar>(color.getAlphaF())   //
  };
}

impeller::SamplerDescriptor ToSamplerDescriptor(
    const appcode::DlImageSampling options) {
  impeller::SamplerDescriptor desc;
  switch (options) {
    case appcode::DlImageSampling::kNearestNeighbor:
      desc.min_filter = desc.mag_filter = impeller::MinMagFilter::kNearest;
      desc.mip_filter = impeller::MipFilter::kBase;
      desc.label = "Nearest Sampler";
      break;
    case appcode::DlImageSampling::kLinear:
      desc.min_filter = desc.mag_filter = impeller::MinMagFilter::kLinear;
      desc.mip_filter = impeller::MipFilter::kBase;
      desc.label = "Linear Sampler";
      break;
    case appcode::DlImageSampling::kCubic:
    case appcode::DlImageSampling::kMipmapLinear:
      desc.min_filter = desc.mag_filter = impeller::MinMagFilter::kLinear;
      desc.mip_filter = impeller::MipFilter::kLinear;
      desc.label = "Mipmap Linear Sampler";
      break;
  }
  return desc;
}

}  // namespace skia_conversions
}  // namespace impeller
