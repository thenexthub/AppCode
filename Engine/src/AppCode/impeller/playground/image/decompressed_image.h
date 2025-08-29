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

#ifndef APPCODE_IMPELLER_PLAYGROUND_IMAGE_DECOMPRESSED_IMAGE_H_
#define APPCODE_IMPELLER_PLAYGROUND_IMAGE_DECOMPRESSED_IMAGE_H_

#include <memory>

#include "appcode/fml/mapping.h"
#include "impeller/geometry/size.h"

namespace impeller {

class DecompressedImage {
 public:
  enum class Format {
    kInvalid,
    kGrey,
    kGreyAlpha,
    kRGB,
    kRGBA,
  };

  DecompressedImage();

  DecompressedImage(ISize size,
                    Format format,
                    std::shared_ptr<const fml::Mapping> allocation);

  ~DecompressedImage();

  const ISize& GetSize() const;

  bool IsValid() const;

  Format GetFormat() const;

  const std::shared_ptr<const fml::Mapping>& GetAllocation() const;

  DecompressedImage ConvertToRGBA() const;

 private:
  ISize size_;
  Format format_ = Format::kInvalid;
  std::shared_ptr<const fml::Mapping> allocation_;
  bool is_valid_ = false;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_PLAYGROUND_IMAGE_DECOMPRESSED_IMAGE_H_
