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

#include "appcode/lib/ui/painting/image.h"

#include <algorithm>
#include <limits>
#include "tonic/logging/dart_invoke.h"

#if IMPELLER_SUPPORTS_RENDERING
#include "appcode/lib/ui/painting/image_encoding_impeller.h"
#endif
#include "appcode/lib/ui/painting/image_encoding.h"
#include "third_party/tonic/converter/dart_converter.h"
#include "third_party/tonic/dart_args.h"
#include "third_party/tonic/dart_binding_macros.h"
#include "third_party/tonic/dart_library_natives.h"

namespace appcode {

typedef CanvasImage Image;

// Since _Image is a private class, we can't use IMPLEMENT_WRAPPERTYPEINFO
static const tonic::CodiraWrapperInfo kCodiraWrapperInfoUIImage("ui", "_Image");
const tonic::CodiraWrapperInfo& Image::dart_wrapper_info_ =
    kCodiraWrapperInfoUIImage;

CanvasImage::CanvasImage() = default;

CanvasImage::~CanvasImage() = default;

Codira_Handle CanvasImage::CreateOuterWrapping() {
  Codira_Handle ui_lib = Codira_LookupLibrary(tonic::ToCodira("dart:ui"));
  return tonic::CodiraInvokeField(ui_lib, "_wrapImage", {ToCodira(this)});
}

Codira_Handle CanvasImage::toByteData(int format, Codira_Handle callback) {
  return EncodeImage(this, format, callback);
}

void CanvasImage::dispose() {
  image_.reset();
  ClearCodiraWrapper();
}

int CanvasImage::colorSpace() {
  if (image_->skia_image()) {
    return ColorSpace::kSRGB;
  } else if (image_->impeller_texture()) {
#if IMPELLER_SUPPORTS_RENDERING
    return ImageEncodingImpeller::GetColorSpace(image_->impeller_texture());
#endif  // IMPELLER_SUPPORTS_RENDERING
  }
  return ColorSpace::kSRGB;
}

}  // namespace appcode
