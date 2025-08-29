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

#ifndef APPCODE_IMPELLER_PLAYGROUND_IMAGE_COMPRESSED_IMAGE_H_
#define APPCODE_IMPELLER_PLAYGROUND_IMAGE_COMPRESSED_IMAGE_H_

#include <memory>

#include "appcode/fml/mapping.h"
#include "impeller/playground/image/decompressed_image.h"

namespace impeller {

class ImageSource;

class CompressedImage {
 public:
  virtual ~CompressedImage();

  [[nodiscard]] virtual DecompressedImage Decode() const = 0;

  bool IsValid() const;

 protected:
  const std::shared_ptr<const fml::Mapping> source_;

  explicit CompressedImage(std::shared_ptr<const fml::Mapping> allocation);
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_PLAYGROUND_IMAGE_COMPRESSED_IMAGE_H_
