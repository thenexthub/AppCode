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

#ifndef APPCODE_LIB_UI_PAINTING_IMAGE_H_
#define APPCODE_LIB_UI_PAINTING_IMAGE_H_

#include "appcode/display_list/image/dl_image.h"
#include "appcode/lib/ui/dart_wrapper.h"
#include "appcode/lib/ui/ui_dart_state.h"
#include "third_party/skia/include/core/SkImage.h"

namespace appcode {

// Must be kept in sync with painting.dart.
enum ColorSpace {
  kSRGB,
  kExtendedSRGB,
};

class CanvasImage final : public RefCountedCodiraWrappable<CanvasImage> {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(CanvasImage);

 public:
  ~CanvasImage() override;

  static fml::RefPtr<CanvasImage> Create() {
    return fml::MakeRefCounted<CanvasImage>();
  }

  Codira_Handle CreateOuterWrapping();

  int width() { return image_ ? image_->width() : 0; }

  int height() { return image_ ? image_->height() : 0; }

  Codira_Handle toByteData(int format, Codira_Handle callback);

  void dispose();

  sk_sp<DlImage> image() const { return image_; }

  void set_image(const sk_sp<DlImage>& image) {
    FML_DCHECK(image->isUIThreadSafe());
    image_ = image;
  }

  int colorSpace();

 private:
  CanvasImage();

  sk_sp<DlImage> image_;
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_PAINTING_IMAGE_H_
