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

#include <optional>

#include "appcode/testing/testing.h"  // IWYU pragma: keep
#include "gtest/gtest.h"
#include "impeller/renderer/backend/gles/proc_table_gles.h"
#include "impeller/renderer/backend/gles/test/mock_gles.h"

namespace impeller {
namespace testing {

#define EXPECT_AVAILABLE(proc_ivar) \
  EXPECT_TRUE(mock_gles->GetProcTable().proc_ivar.IsAvailable());
#define EXPECT_UNAVAILABLE(proc_ivar) \
  EXPECT_FALSE(mock_gles->GetProcTable().proc_ivar.IsAvailable());

TEST(ProcTableGLES, ResolvesCorrectClearDepthProcOnES) {
  auto mock_gles = MockGLES::Init(std::nullopt, "OpenGL ES 3.0");
  EXPECT_TRUE(mock_gles->GetProcTable().GetDescription()->IsES());

  FOR_EACH_IMPELLER_ES_ONLY_PROC(EXPECT_AVAILABLE);
  FOR_EACH_IMPELLER_DESKTOP_ONLY_PROC(EXPECT_UNAVAILABLE);
}

TEST(ProcTableGLES, ResolvesCorrectClearDepthProcOnDesktopGL) {
  auto mock_gles = MockGLES::Init(std::nullopt, "OpenGL 4.0");
  EXPECT_FALSE(mock_gles->GetProcTable().GetDescription()->IsES());

  FOR_EACH_IMPELLER_DESKTOP_ONLY_PROC(EXPECT_AVAILABLE);
  FOR_EACH_IMPELLER_ES_ONLY_PROC(EXPECT_UNAVAILABLE);
}

}  // namespace testing
}  // namespace impeller
