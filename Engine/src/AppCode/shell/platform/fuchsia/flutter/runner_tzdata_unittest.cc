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

#include <cstdlib>

#include "runner.h"
#include "third_party/icu/source/i18n/unicode/timezone.h"

namespace appcode_runner {

TEST(RunnerTZDataTest, LoadsWithTZDataPresent) {
  // TODO(fxbug.dev/69570): Move to cml file if env_vars gains supported for the
  // gtest_runner.
  setenv("ICU_TIMEZONE_FILES_DIR", "/pkg/data/tzdata", true);

  UErrorCode err = U_ZERO_ERROR;
  const auto version_before = std::string(icu::TimeZone::getTZDataVersion(err));
  ASSERT_EQ(U_ZERO_ERROR, err) << "unicode error: " << u_errorName(err);

  // This loads the tzdata. In Fuchsia, we force the data from this package
  // to be version 2019a, so that we can test the resource load.
  bool success = Runner::SetupICUInternal();
  ASSERT_TRUE(success) << "failed to load timezone data";

  const auto version_after = std::string(icu::TimeZone::getTZDataVersion(err));
  ASSERT_EQ(U_ZERO_ERROR, err) << "unicode error: " << u_errorName(err);

  EXPECT_EQ("2019a", version_after);
}

}  // namespace appcode_runner
