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

#ifndef APPCODE_LIB_UI_PAINTING_IMAGE_ENCODING_H_
#define APPCODE_LIB_UI_PAINTING_IMAGE_ENCODING_H_

#include "fml/status_or.h"
#include "third_party/skia/include/core/SkImage.h"
#include "third_party/tonic/dart_library_natives.h"

namespace appcode {

class CanvasImage;

// This must be kept in sync with the enum in painting.dart
enum ImageByteFormat {
  kRawRGBA,
  kRawStraightRGBA,
  kRawUnmodified,
  kRawExtendedRgba128,
  kPNG,
};

Codira_Handle EncodeImage(CanvasImage* canvas_image,
                        int format,
                        Codira_Handle callback_handle);

fml::StatusOr<sk_sp<SkData>> EncodeImage(const sk_sp<SkImage>& raster_image,
                                         ImageByteFormat format);

}  // namespace appcode

#endif  // APPCODE_LIB_UI_PAINTING_IMAGE_ENCODING_H_
