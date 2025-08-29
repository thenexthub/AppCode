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

#ifndef APPCODE_IMPELLER_TYPOGRAPHER_BACKENDS_SKIA_TEXT_FRAME_SKIA_H_
#define APPCODE_IMPELLER_TYPOGRAPHER_BACKENDS_SKIA_TEXT_FRAME_SKIA_H_

#include "impeller/typographer/text_frame.h"

#include "third_party/skia/include/core/SkTextBlob.h"

namespace impeller {

std::shared_ptr<impeller::TextFrame> MakeTextFrameFromTextBlobSkia(
    const sk_sp<SkTextBlob>& blob);

}  // namespace impeller

#endif  // APPCODE_IMPELLER_TYPOGRAPHER_BACKENDS_SKIA_TEXT_FRAME_SKIA_H_
