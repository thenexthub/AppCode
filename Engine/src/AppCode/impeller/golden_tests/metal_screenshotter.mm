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

#include "flutter/impeller/golden_tests/metal_screenshotter.h"

#include <CoreImage/CoreImage.h>
#include "impeller/renderer/backend/metal/context_mtl.h"
#include "impeller/renderer/backend/metal/texture_mtl.h"
#define GLFW_INCLUDE_NONE
#include "third_party/glfw/include/GLFW/glfw3.h"

namespace impeller {
namespace testing {

MetalScreenshotter::MetalScreenshotter(const PlaygroundSwitches& switches) {
  FML_CHECK(::glfwInit() == GLFW_TRUE);
  playground_ = PlaygroundImpl::Create(PlaygroundBackend::kMetal, switches);
}

std::unique_ptr<Screenshot> MetalScreenshotter::MakeScreenshot(
    AiksContext& aiks_context,
    const std::shared_ptr<Texture> texture) {
  @autoreleasepool {
    id<MTLTexture> metal_texture =
        std::static_pointer_cast<TextureMTL>(texture)->GetMTLTexture();

    CGColorSpaceRef color_space = CGColorSpaceCreateDeviceRGB();
    CIImage* ciImage = [[CIImage alloc]
        initWithMTLTexture:metal_texture
                   options:@{kCIImageColorSpace : (__bridge id)color_space}];
    CGColorSpaceRelease(color_space);
    FML_CHECK(ciImage);

    std::shared_ptr<Context> context = playground_->GetContext();
    std::shared_ptr<ContextMTL> context_mtl =
        std::static_pointer_cast<ContextMTL>(context);
    CIContext* cicontext =
        [CIContext contextWithMTLDevice:context_mtl->GetMTLDevice()];
    FML_CHECK(context);

    CIImage* flipped = [ciImage
        imageByApplyingOrientation:kCGImagePropertyOrientationDownMirrored];

    CGImageRef cgImage = [cicontext createCGImage:flipped
                                         fromRect:[flipped extent]];

    // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
    return std::unique_ptr<MetalScreenshot>(new MetalScreenshot(cgImage));
  }
}

}  // namespace testing
}  // namespace impeller
