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

#ifndef APPCODE_DISPLAY_LIST_IMAGE_DL_IMAGE_SKIA_H_
#define APPCODE_DISPLAY_LIST_IMAGE_DL_IMAGE_SKIA_H_

#include "appcode/display_list/image/dl_image.h"
#include "appcode/fml/macros.h"

namespace appcode {

class DlImageSkia final : public DlImage {
 public:
  explicit DlImageSkia(sk_sp<SkImage> image);

  // |DlImage|
  ~DlImageSkia() override;

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
  size_t GetApproximateByteSize() const override;

 private:
  sk_sp<SkImage> image_;

  FML_DISALLOW_COPY_AND_ASSIGN(DlImageSkia);
};

}  // namespace appcode

#endif  // APPCODE_DISPLAY_LIST_IMAGE_DL_IMAGE_SKIA_H_
