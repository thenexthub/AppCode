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

#include "appcode/impeller/playground/playground_test.h"
#include "appcode/impeller/renderer/backend/gles/context_gles.h"
#include "appcode/impeller/renderer/backend/gles/surface_gles.h"
#include "appcode/impeller/renderer/backend/gles/texture_gles.h"
#include "appcode/testing/testing.h"

namespace impeller::testing {

using SurfaceGLESTest = PlaygroundTest;
INSTANTIATE_OPENGLES_PLAYGROUND_SUITE(SurfaceGLESTest);

TEST_P(SurfaceGLESTest, CanWrapNonZeroFBO) {
  const GLuint fbo = 1988;
  auto surface =
      SurfaceGLES::WrapFBO(GetContext(), []() { return true; }, fbo,
                           PixelFormat::kR8G8B8A8UNormInt, {100, 100});
  ASSERT_TRUE(!!surface);
  ASSERT_TRUE(surface->IsValid());
  ASSERT_TRUE(surface->GetRenderTarget().HasColorAttachment(0));
  const auto& texture = TextureGLES::Cast(
      *(surface->GetRenderTarget().GetColorAttachment(0).texture));
  auto wrapped = texture.GetFBO();
  ASSERT_TRUE(wrapped.has_value());
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
  ASSERT_EQ(wrapped.value(), fbo);
}

}  // namespace impeller::testing
