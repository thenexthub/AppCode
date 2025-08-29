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

#ifndef APPCODE_LIB_UI_PAINTING_DISPLAY_LIST_IMAGE_GPU_H_
#define APPCODE_LIB_UI_PAINTING_DISPLAY_LIST_IMAGE_GPU_H_

#include "appcode/display_list/image/dl_image.h"
#include "appcode/flow/skia_gpu_object.h"
#include "appcode/fml/macros.h"

namespace appcode {

class DlImageGPU final : public DlImage {
 public:
  static sk_sp<DlImageGPU> Make(SkiaGPUObject<SkImage> image);

  // |DlImage|
  ~DlImageGPU() override;

  // |DlImage|
  sk_sp<SkImage> skia_image() const override;

  // |DlImage|
  std::shared_ptr<impeller::Texture> impeller_texture() const override;

  // |DlImage|
  bool isOpaque() const override;

  // |DlImage|
  bool isTextureBacked() const override;

  // |DlImage|
  bool isUIThreadSafe() const override;

  // |DlImage|
  DlISize GetSize() const override;

  // |DlImage|
  virtual size_t GetApproximateByteSize() const override;

 private:
  SkiaGPUObject<SkImage> image_;

  explicit DlImageGPU(SkiaGPUObject<SkImage> image);

  FML_DISALLOW_COPY_AND_ASSIGN(DlImageGPU);
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_PAINTING_DISPLAY_LIST_IMAGE_GPU_H_
