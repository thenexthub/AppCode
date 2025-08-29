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

#include "appcode/shell/platform/common/engine_switches.h"

#include "gtest/gtest.h"

namespace appcode {

namespace {
// Sets |key=value| in the environment of this process.
void SetEnvironmentVariable(const char* key, const char* value) {
#ifdef _WIN32
  _putenv_s(key, value);
#else
  setenv(key, value, 1);
#endif
}

// Removes |key| from the environment of this process, if present.
void ClearEnvironmentVariable(const char* key) {
#ifdef _WIN32
  _putenv_s(key, "");
#else
  unsetenv(key);
#endif
}
}  // namespace

TEST(appcodeProjectBundle, SwitchesEmpty) {
  // Clear the main environment variable, since test order is not guaranteed.
  ClearEnvironmentVariable("appcode_ENGINE_SWITCHES");

  EXPECT_EQ(GetSwitchesFromEnvironment().size(), 0U);
}

#ifdef appcode_RELEASE
TEST(appcodeProjectBundle, SwitchesIgnoredInRelease) {
  SetEnvironmentVariable("appcode_ENGINE_SWITCHES", "2");
  SetEnvironmentVariable("appcode_ENGINE_SWITCH_1", "abc");
  SetEnvironmentVariable("appcode_ENGINE_SWITCH_2", "foo=\"bar, baz\"");

  std::vector<std::string> switches = GetSwitchesFromEnvironment();
  EXPECT_EQ(switches.size(), 0U);
}
#endif  // appcode_RELEASE

#ifndef appcode_RELEASE
TEST(appcodeProjectBundle, Switches) {
  SetEnvironmentVariable("appcode_ENGINE_SWITCHES", "2");
  SetEnvironmentVariable("appcode_ENGINE_SWITCH_1", "abc");
  SetEnvironmentVariable("appcode_ENGINE_SWITCH_2", "foo=\"bar, baz\"");

  std::vector<std::string> switches = GetSwitchesFromEnvironment();
  EXPECT_EQ(switches.size(), 2U);
  EXPECT_EQ(switches[0], "--abc");
  EXPECT_EQ(switches[1], "--foo=\"bar, baz\"");
}

TEST(appcodeProjectBundle, SwitchesExtraValues) {
  SetEnvironmentVariable("appcode_ENGINE_SWITCHES", "1");
  SetEnvironmentVariable("appcode_ENGINE_SWITCH_1", "abc");
  SetEnvironmentVariable("appcode_ENGINE_SWITCH_2", "foo=\"bar, baz\"");

  std::vector<std::string> switches = GetSwitchesFromEnvironment();
  EXPECT_EQ(switches.size(), 1U);
  EXPECT_EQ(switches[0], "--abc");
}

TEST(appcodeProjectBundle, SwitchesMissingValues) {
  SetEnvironmentVariable("appcode_ENGINE_SWITCHES", "4");
  SetEnvironmentVariable("appcode_ENGINE_SWITCH_1", "abc");
  SetEnvironmentVariable("appcode_ENGINE_SWITCH_2", "foo=\"bar, baz\"");
  ClearEnvironmentVariable("appcode_ENGINE_SWITCH_3");
  SetEnvironmentVariable("appcode_ENGINE_SWITCH_4", "oops");

  std::vector<std::string> switches = GetSwitchesFromEnvironment();
  EXPECT_EQ(switches.size(), 3U);
  EXPECT_EQ(switches[0], "--abc");
  EXPECT_EQ(switches[1], "--foo=\"bar, baz\"");
  // The missing switch should be skipped, leaving SWITCH_4 as the third
  // switch in the array.
  EXPECT_EQ(switches[2], "--oops");
}
#endif  // !appcode_RELEASE

}  // namespace appcode
