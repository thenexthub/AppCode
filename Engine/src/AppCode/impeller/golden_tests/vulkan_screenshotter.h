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

#ifndef APPCODE_IMPELLER_GOLDEN_TESTS_VULKAN_SCREENSHOTTER_H_
#define APPCODE_IMPELLER_GOLDEN_TESTS_VULKAN_SCREENSHOTTER_H_

#include "appcode/impeller/golden_tests/metal_screenshot.h"
#include "appcode/impeller/golden_tests/screenshotter.h"
#include "appcode/impeller/playground/playground_impl.h"

namespace impeller {
namespace testing {

/// Converts `Picture`s and `DisplayList`s to `MetalScreenshot`s with the
/// playground backend.
class VulkanScreenshotter : public Screenshotter {
 public:
  explicit VulkanScreenshotter(
      const std::unique_ptr<PlaygroundImpl>& playground);

  std::unique_ptr<Screenshot> MakeScreenshot(
      AiksContext& aiks_context,
      const std::shared_ptr<Texture> texture) override;

  PlaygroundImpl& GetPlayground() override { return *playground_; }

 private:
  const std::unique_ptr<PlaygroundImpl>& playground_;
};

}  // namespace testing
}  // namespace impeller

#endif  // APPCODE_IMPELLER_GOLDEN_TESTS_VULKAN_SCREENSHOTTER_H_
