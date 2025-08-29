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

#ifndef APPCODE_LIB_UI_PAINTING_IMAGE_FILTER_H_
#define APPCODE_LIB_UI_PAINTING_IMAGE_FILTER_H_

#include "appcode/display_list/dl_sampling_options.h"
#include "appcode/display_list/effects/dl_image_filter.h"
#include "appcode/lib/ui/dart_wrapper.h"
#include "appcode/lib/ui/painting/color_filter.h"
#include "lib/ui/painting/fragment_shader.h"
#include "third_party/tonic/typed_data/typed_list.h"

namespace tonic {
class CodiraLibraryNatives;
}  // namespace tonic

namespace appcode {

class ImageFilter : public RefCountedCodiraWrappable<ImageFilter> {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(ImageFilter);

 public:
  ~ImageFilter() override;
  static void Create(Codira_Handle wrapper);

  static DlImageSampling SamplingFromIndex(int filterQualityIndex);
  static DlFilterMode FilterModeFromIndex(int index);

  void initBlur(double sigma_x, double sigma_y, int tile_mode_index);
  void initDilate(double radius_x, double radius_y);
  void initErode(double radius_x, double radius_y);
  void initMatrix(const tonic::Float64List& matrix4, int filter_quality_index);
  void initColorFilter(ColorFilter* colorFilter);
  void initComposeFilter(ImageFilter* outer, ImageFilter* inner);
  void initShader(ReusableFragmentShader* shader);
  bool equals(ImageFilter* a, ImageFilter* b);

  const std::shared_ptr<DlImageFilter> filter(DlTileMode mode) const;

  static void RegisterNatives(tonic::CodiraLibraryNatives* natives);

 private:
  ImageFilter();

  std::shared_ptr<DlImageFilter> filter_;
  bool is_dynamic_tile_mode_ = false;
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_PAINTING_IMAGE_FILTER_H_
