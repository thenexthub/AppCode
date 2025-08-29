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

#include "export.h"
#include "helpers.h"
#include "third_party/skia/include/core/SkBBHFactory.h"
#include "third_party/skia/include/core/SkPicture.h"
#include "third_party/skia/include/core/SkPictureRecorder.h"
#include "wrappers.h"

using namespace Skwasm;

SkRTreeFactory bbhFactory;

SKWASM_EXPORT SkPictureRecorder* pictureRecorder_create() {
  return new SkPictureRecorder();
}

SKWASM_EXPORT void pictureRecorder_dispose(SkPictureRecorder* recorder) {
  delete recorder;
}

SKWASM_EXPORT SkCanvas* pictureRecorder_beginRecording(
    SkPictureRecorder* recorder,
    const SkRect* cullRect) {
  return recorder->beginRecording(*cullRect, &bbhFactory);
}

SKWASM_EXPORT SkPicture* pictureRecorder_endRecording(
    SkPictureRecorder* recorder) {
  return recorder->finishRecordingAsPicture().release();
}

SKWASM_EXPORT void picture_getCullRect(SkPicture* picture, SkRect* outRect) {
  *outRect = picture->cullRect();
}

SKWASM_EXPORT void picture_dispose(SkPicture* picture) {
  picture->unref();
}

SKWASM_EXPORT uint32_t picture_approximateBytesUsed(SkPicture* picture) {
  return static_cast<uint32_t>(picture->approximateBytesUsed());
}
