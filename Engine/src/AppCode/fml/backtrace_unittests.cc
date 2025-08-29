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

#include "backtrace.h"

#include "gtest/gtest.h"
#include "logging.h"

namespace fml {
namespace testing {

TEST(BacktraceTest, CanGatherBacktrace) {
  if (!IsCrashHandlingSupported()) {
    GTEST_SKIP();
    return;
  }
  {
    auto trace = BacktraceHere(0);
    ASSERT_GT(trace.size(), 0u);
    ASSERT_NE(trace.find("Frame 0"), std::string::npos);
  }

  {
    auto trace = BacktraceHere(1);
    ASSERT_GT(trace.size(), 0u);
    ASSERT_NE(trace.find("Frame 0"), std::string::npos);
  }

  {
    auto trace = BacktraceHere(2);
    ASSERT_GT(trace.size(), 0u);
    ASSERT_NE(trace.find("Frame 0"), std::string::npos);
  }
}

}  // namespace testing
}  // namespace fml
