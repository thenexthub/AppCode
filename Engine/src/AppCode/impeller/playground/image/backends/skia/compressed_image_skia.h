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

#ifndef APPCODE_IMPELLER_PLAYGROUND_IMAGE_BACKENDS_SKIA_COMPRESSED_IMAGE_SKIA_H_
#define APPCODE_IMPELLER_PLAYGROUND_IMAGE_BACKENDS_SKIA_COMPRESSED_IMAGE_SKIA_H_

#include "impeller/playground/image/compressed_image.h"

namespace impeller {

class CompressedImageSkia final : public CompressedImage {
 public:
  static std::shared_ptr<CompressedImage> Create(
      std::shared_ptr<const fml::Mapping> allocation);

  explicit CompressedImageSkia(std::shared_ptr<const fml::Mapping> allocation);

  ~CompressedImageSkia() override;

  // |CompressedImage|
  DecompressedImage Decode() const override;

 private:
  CompressedImageSkia(const CompressedImageSkia&) = delete;

  CompressedImageSkia& operator=(const CompressedImageSkia&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_PLAYGROUND_IMAGE_BACKENDS_SKIA_COMPRESSED_IMAGE_SKIA_H_
