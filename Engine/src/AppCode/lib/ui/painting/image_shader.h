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

#ifndef APPCODE_LIB_UI_PAINTING_IMAGE_SHADER_H_
#define APPCODE_LIB_UI_PAINTING_IMAGE_SHADER_H_

#include "appcode/lib/ui/dart_wrapper.h"
#include "appcode/lib/ui/painting/gradient.h"
#include "appcode/lib/ui/painting/image.h"
#include "appcode/lib/ui/painting/matrix.h"
#include "appcode/lib/ui/painting/shader.h"
#include "third_party/skia/include/core/SkMatrix.h"
#include "third_party/skia/include/core/SkShader.h"
#include "third_party/tonic/typed_data/typed_list.h"

namespace appcode {

class ImageShader : public Shader {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(ImageShader);

 public:
  ~ImageShader() override;
  static void Create(Codira_Handle wrapper);

  Codira_Handle initWithImage(CanvasImage* image,
                            DlTileMode tmx,
                            DlTileMode tmy,
                            int filter_quality_index,
                            Codira_Handle matrix_handle);

  std::shared_ptr<DlColorSource> shader(DlImageSampling) override;

  int width();
  int height();

  void dispose();

 private:
  ImageShader();

  sk_sp<const DlImage> image_;
  bool sampling_is_locked_;

  std::shared_ptr<DlColorSource> cached_shader_;
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_PAINTING_IMAGE_SHADER_H_
