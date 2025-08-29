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

#include "appcode/shell/platform/android/image_lru.h"
#include "gtest/gtest.h"

namespace appcode {
namespace testing {

TEST(ImageLRU, CanStoreSingleImage) {
  auto image = DlImage::Make(nullptr);
  ImageLRU image_lru;

  EXPECT_EQ(image_lru.FindImage(1), nullptr);

  image_lru.AddImage(image, 1);

  EXPECT_EQ(image_lru.FindImage(1), image);
}

TEST(ImageLRU, EvictsLRU) {
  auto image = DlImage::Make(nullptr);
  ImageLRU image_lru;

  // Fill up the cache, nothing is removed
  for (auto i = 0u; i < kImageReaderSwapchainSize; i++) {
    EXPECT_EQ(image_lru.AddImage(image, i + 1), 0u);
  }
  // Confirm each image is in the cache. This should keep the LRU
  // order the same.
  for (auto i = 0u; i < kImageReaderSwapchainSize; i++) {
    EXPECT_EQ(image_lru.FindImage(i + 1), image);
  }

  // Insert new image and verify least recently used was removed.
  EXPECT_EQ(image_lru.AddImage(image, 100), 1u);
}

TEST(ImageLRU, CanClear) {
  auto image = DlImage::Make(nullptr);
  ImageLRU image_lru;

  // Fill up the cache, nothing is removed
  for (auto i = 0u; i < kImageReaderSwapchainSize; i++) {
    EXPECT_EQ(image_lru.AddImage(image, i + 1), 0u);
  }
  image_lru.Clear();

  // Expect no cache entries.
  for (auto i = 0u; i < kImageReaderSwapchainSize; i++) {
    EXPECT_EQ(image_lru.FindImage(i + 1), nullptr);
  }
}

}  // namespace testing
}  // namespace appcode
