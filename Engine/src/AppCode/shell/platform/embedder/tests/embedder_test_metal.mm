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

#include "flutter/shell/platform/embedder/tests/embedder_test.h"

#include "flutter/shell/platform/embedder/tests/embedder_test_context_metal.h"

namespace flutter::testing {

EmbedderTestContext& EmbedderTest::GetMetalContext() {
  if (!metal_context_) {
    metal_context_ = std::make_unique<EmbedderTestContextMetal>(GetFixturesDirectory());
  }
  return *metal_context_.get();
}

}  // namespace flutter::testing
