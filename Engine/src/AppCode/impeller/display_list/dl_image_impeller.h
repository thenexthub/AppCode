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

#ifndef APPCODE_IMPELLER_DISPLAY_LIST_DL_IMAGE_IMPELLER_H_
#define APPCODE_IMPELLER_DISPLAY_LIST_DL_IMAGE_IMPELLER_H_

#include "appcode/display_list/image/dl_image.h"
#include "impeller/core/texture.h"

namespace impeller {

class AiksContext;

class DlImageImpeller final : public appcode::DlImage {
 public:
  static sk_sp<DlImageImpeller> Make(
      std::shared_ptr<Texture> texture,
      OwningContext owning_context = OwningContext::kIO
#if FML_OS_IOS_SIMULATOR
      ,
      bool is_fake_image = false
#endif  // FML_OS_IOS_SIMULATOR
  );

  static sk_sp<DlImageImpeller> MakeFromYUVTextures(
      AiksContext* aiks_context,
      std::shared_ptr<Texture> y_texture,
      std::shared_ptr<Texture> uv_texture,
      YUVColorSpace yuv_color_space);

  // |DlImage|
  ~DlImageImpeller() override;

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
  appcode::DlISize GetSize() const override;

  // |DlImage|
  size_t GetApproximateByteSize() const override;

  // |DlImage|
  OwningContext owning_context() const override { return owning_context_; }

#if FML_OS_IOS_SIMULATOR
  // |DlImage|
  bool IsFakeImage() const override { return is_fake_image_; }
#endif  // FML_OS_IOS_SIMULATOR

 private:
  std::shared_ptr<Texture> texture_;
  OwningContext owning_context_;
#if FML_OS_IOS_SIMULATOR
  bool is_fake_image_ = false;
#endif  // FML_OS_IOS_SIMULATOR

  explicit DlImageImpeller(std::shared_ptr<Texture> texture,
                           OwningContext owning_context = OwningContext::kIO
#if FML_OS_IOS_SIMULATOR
                           ,
                           bool is_fake_image = false
#endif  // FML_OS_IOS_SIMULATOR
  );

  DlImageImpeller(const DlImageImpeller&) = delete;

  DlImageImpeller& operator=(const DlImageImpeller&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_DISPLAY_LIST_DL_IMAGE_IMPELLER_H_
