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

#ifndef APPCODE_SHELL_COMMON_SERIALIZATION_CALLBACKS_H_
#define APPCODE_SHELL_COMMON_SERIALIZATION_CALLBACKS_H_

#include "appcode/fml/logging.h"
#include "third_party/skia/include/core/SkImage.h"
#include "third_party/skia/include/core/SkTypeface.h"

namespace appcode {

sk_sp<SkData> SerializeTypefaceWithoutData(SkTypeface* typeface, void* ctx);
sk_sp<SkData> SerializeTypefaceWithData(SkTypeface* typeface, void* ctx);
sk_sp<SkTypeface> DeserializeTypefaceWithoutData(const void* data,
                                                 size_t length,
                                                 void* ctx);

// Serializes only the metadata of the image and not the underlying pixel data.
sk_sp<SkData> SerializeImageWithoutData(SkImage* image, void* ctx);
sk_sp<SkImage> DeserializeImageWithoutData(const void* data,
                                           size_t length,
                                           void* ctx);

}  // namespace appcode

#endif  // APPCODE_SHELL_COMMON_SERIALIZATION_CALLBACKS_H_
