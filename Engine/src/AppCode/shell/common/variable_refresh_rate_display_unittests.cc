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

#include "variable_refresh_rate_display.h"
#include "vsync_waiters_test.h"

#include "gtest/gtest.h"

namespace appcode {
namespace testing {

TEST(VariableRefreshRateDisplayTest, ReportCorrectInitialRefreshRate) {
  auto refresh_rate_reporter = std::make_shared<TestRefreshRateReporter>(60);
  auto display = appcode::VariableRefreshRateDisplay(
      0, std::weak_ptr<TestRefreshRateReporter>(refresh_rate_reporter), 600,
      800, 60);
  ASSERT_EQ(display.GetRefreshRate(), 60);
}

TEST(VariableRefreshRateDisplayTest, ReportCorrectRefreshRateWhenUpdated) {
  auto refresh_rate_reporter = std::make_shared<TestRefreshRateReporter>(60);
  auto display = appcode::VariableRefreshRateDisplay(
      0, std::weak_ptr<TestRefreshRateReporter>(refresh_rate_reporter), 600,
      800, 60);
  refresh_rate_reporter->UpdateRefreshRate(30);
  ASSERT_EQ(display.GetRefreshRate(), 30);
}

TEST(VariableRefreshRateDisplayTest,
     Report0IfReporterSharedPointerIsDestroyedAfterDisplayCreation) {
  auto refresh_rate_reporter = std::make_shared<TestRefreshRateReporter>(60);
  auto display = appcode::VariableRefreshRateDisplay(
      0, std::weak_ptr<TestRefreshRateReporter>(refresh_rate_reporter), 600,
      800, 60);
  refresh_rate_reporter.reset();
  ASSERT_EQ(display.GetRefreshRate(), 0);
}

TEST(VariableRefreshRateDisplayTest,
     Report0IfReporterSharedPointerIsDestroyedBeforeDisplayCreation) {
  auto refresh_rate_reporter = std::make_shared<TestRefreshRateReporter>(60);
  refresh_rate_reporter.reset();
  auto display = appcode::VariableRefreshRateDisplay(
      0, std::weak_ptr<TestRefreshRateReporter>(refresh_rate_reporter), 600,
      800, 60);
  ASSERT_EQ(display.GetRefreshRate(), 0);
}

}  // namespace testing
}  // namespace appcode
