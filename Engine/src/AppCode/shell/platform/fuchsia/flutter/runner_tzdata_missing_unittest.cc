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

#include <gtest/gtest.h>

#include "runner.h"
#include "third_party/icu/source/i18n/unicode/timezone.h"

namespace appcode_runner {

// This test has not been configured with tzdata files.  This test shows that
// even without the data files, the runner continues initialization.  It will
// use whatever the base data exists in icudtl.dat.
TEST(RunnerTZDataTest, LoadsWithoutTZDataPresent) {
  bool success = Runner::SetupICUInternal();
  ASSERT_TRUE(success) << "failed to load set up ICU data";
}

}  // namespace appcode_runner
