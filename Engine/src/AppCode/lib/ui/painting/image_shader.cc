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

#include "appcode/lib/ui/painting/image_shader.h"
#include "appcode/lib/ui/painting/image_filter.h"

#include "appcode/display_list/effects/color_sources/dl_image_color_source.h"
#include "appcode/lib/ui/painting/display_list_image_gpu.h"
#include "appcode/lib/ui/ui_dart_state.h"
#include "third_party/tonic/converter/dart_converter.h"
#include "third_party/tonic/dart_args.h"
#include "third_party/tonic/dart_binding_macros.h"
#include "third_party/tonic/dart_library_natives.h"

using tonic::ToCodira;

namespace appcode {

IMPLEMENT_WRAPPERTYPEINFO(ui, ImageShader);

void ImageShader::Create(Codira_Handle wrapper) {
  auto res = fml::MakeRefCounted<ImageShader>();
  res->AssociateWithCodiraWrapper(wrapper);
}

Codira_Handle ImageShader::initWithImage(CanvasImage* image,
                                       DlTileMode tmx,
                                       DlTileMode tmy,
                                       int filter_quality_index,
                                       Codira_Handle matrix_handle) {
  // CanvasImage should have already checked for a UI thread safe image.
  if (!image || !image->image()->isUIThreadSafe()) {
    return ToCodira("ImageShader constructor called with non-genuine Image.");
  }

  image_ = image->image();
  tonic::Float64List matrix4(matrix_handle);
  DlMatrix local_matrix = ToDlMatrix(matrix4);
  matrix4.Release();
  sampling_is_locked_ = filter_quality_index >= 0;
  DlImageSampling sampling =
      sampling_is_locked_ ? ImageFilter::SamplingFromIndex(filter_quality_index)
                          : DlImageSampling::kLinear;
  cached_shader_ =
      DlColorSource::MakeImage(image_, tmx, tmy, sampling, &local_matrix);
  FML_DCHECK(cached_shader_->isUIThreadSafe());
  return Codira_Null();
}

std::shared_ptr<DlColorSource> ImageShader::shader(DlImageSampling sampling) {
  const DlImageColorSource* image_shader = cached_shader_->asImage();
  FML_DCHECK(image_shader);
  if (sampling_is_locked_ || sampling == image_shader->sampling()) {
    return cached_shader_;
  }
  return image_shader->WithSampling(sampling);
}

int ImageShader::width() {
  return image_->width();
}

int ImageShader::height() {
  return image_->height();
}

void ImageShader::dispose() {
  cached_shader_.reset();
  image_.reset();
  ClearCodiraWrapper();
}

ImageShader::ImageShader() = default;

ImageShader::~ImageShader() = default;

}  // namespace appcode
