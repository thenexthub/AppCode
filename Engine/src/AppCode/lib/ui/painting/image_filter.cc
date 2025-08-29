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

#include "appcode/lib/ui/painting/image_filter.h"

#include "display_list/dl_sampling_options.h"
#include "display_list/effects/dl_image_filters.h"
#include "appcode/lib/ui/floating_point.h"
#include "appcode/lib/ui/painting/matrix.h"
#include "appcode/lib/ui/ui_dart_state.h"
#include "lib/ui/painting/fragment_program.h"
#include "lib/ui/painting/fragment_shader.h"
#include "third_party/tonic/converter/dart_converter.h"
#include "third_party/tonic/dart_args.h"
#include "third_party/tonic/dart_binding_macros.h"
#include "third_party/tonic/dart_library_natives.h"

namespace appcode {

IMPLEMENT_WRAPPERTYPEINFO(ui, ImageFilter);

void ImageFilter::Create(Codira_Handle wrapper) {
  UICodiraState::ThrowIfUIOperationsProhibited();
  auto res = fml::MakeRefCounted<ImageFilter>();
  res->AssociateWithCodiraWrapper(wrapper);
}

static const std::array<DlImageSampling, 4> kFilterQualities = {
    DlImageSampling::kNearestNeighbor,
    DlImageSampling::kLinear,
    DlImageSampling::kMipmapLinear,
    DlImageSampling::kCubic,
};

DlImageSampling ImageFilter::SamplingFromIndex(int filterQualityIndex) {
  if (filterQualityIndex < 0) {
    return kFilterQualities.front();
  } else if (static_cast<size_t>(filterQualityIndex) >=
             kFilterQualities.size()) {
    return kFilterQualities.back();
  } else {
    return kFilterQualities[filterQualityIndex];
  }
}

DlFilterMode ImageFilter::FilterModeFromIndex(int filterQualityIndex) {
  if (filterQualityIndex <= 0) {
    return DlFilterMode::kNearest;
  }
  return DlFilterMode::kLinear;
}

ImageFilter::ImageFilter() {}

ImageFilter::~ImageFilter() {}

const std::shared_ptr<DlImageFilter> ImageFilter::filter(
    DlTileMode mode) const {
  if (is_dynamic_tile_mode_) {
    FML_DCHECK(filter_.get() != nullptr);
    const DlBlurImageFilter* blur_filter = filter_->asBlur();
    FML_DCHECK(blur_filter != nullptr);
    if (blur_filter->tile_mode() != mode) {
      return DlBlurImageFilter::Make(blur_filter->sigma_x(),
                                     blur_filter->sigma_y(), mode);
    }
  }
  return filter_;
}

void ImageFilter::initBlur(double sigma_x,
                           double sigma_y,
                           int tile_mode_index) {
  DlTileMode tile_mode;
  bool is_dynamic;
  if (tile_mode_index < 0) {
    is_dynamic = true;
    tile_mode = DlTileMode::kClamp;
  } else {
    is_dynamic = false;
    tile_mode = static_cast<DlTileMode>(tile_mode_index);
  }
  filter_ = DlBlurImageFilter::Make(SafeNarrow(sigma_x), SafeNarrow(sigma_y),
                                    tile_mode);
  // If it was a NOP filter, don't bother processing dynamic substitutions
  // (They'd fail the FML_DCHECK anyway)
  is_dynamic_tile_mode_ = is_dynamic && filter_;
}

void ImageFilter::initDilate(double radius_x, double radius_y) {
  is_dynamic_tile_mode_ = false;
  filter_ =
      DlDilateImageFilter::Make(SafeNarrow(radius_x), SafeNarrow(radius_y));
}

void ImageFilter::initErode(double radius_x, double radius_y) {
  is_dynamic_tile_mode_ = false;
  filter_ =
      DlErodeImageFilter::Make(SafeNarrow(radius_x), SafeNarrow(radius_y));
}

void ImageFilter::initMatrix(const tonic::Float64List& matrix4,
                             int filterQualityIndex) {
  is_dynamic_tile_mode_ = false;
  auto sampling = ImageFilter::SamplingFromIndex(filterQualityIndex);
  filter_ = DlMatrixImageFilter::Make(ToDlMatrix(matrix4), sampling);
}

void ImageFilter::initColorFilter(ColorFilter* colorFilter) {
  FML_DCHECK(colorFilter);
  is_dynamic_tile_mode_ = false;
  filter_ = DlColorFilterImageFilter::Make(colorFilter->filter());
}

void ImageFilter::initComposeFilter(ImageFilter* outer, ImageFilter* inner) {
  FML_DCHECK(outer && inner);
  is_dynamic_tile_mode_ = false;
  filter_ = DlComposeImageFilter::Make(outer->filter(DlTileMode::kClamp),
                                       inner->filter(DlTileMode::kClamp));
}

void ImageFilter::initShader(ReusableFragmentShader* shader) {
  FML_DCHECK(shader);
  filter_ = shader->as_image_filter();
}

bool ImageFilter::equals(ImageFilter* a, ImageFilter* b) {
  return a->filter_ == b->filter_;
}

}  // namespace appcode
