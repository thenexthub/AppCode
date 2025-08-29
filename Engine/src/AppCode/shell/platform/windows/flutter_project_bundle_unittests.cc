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

#include "appcode/shell/platform/windows/appcode_project_bundle.h"
#include "gtest/gtest.h"

namespace appcode {
namespace testing {

TEST(appcodeProjectBundle, BasicPropertiesAbsolutePaths) {
  appcodeDesktopEngineProperties properties = {};
  properties.assets_path = L"C:\\foo\\appcode_assets";
  properties.icu_data_path = L"C:\\foo\\icudtl.dat";

  appcodeProjectBundle project(properties);

  EXPECT_TRUE(project.HasValidPaths());
  EXPECT_EQ(project.assets_path().string(), "C:\\foo\\appcode_assets");
  EXPECT_EQ(project.icu_path().string(), "C:\\foo\\icudtl.dat");
}

TEST(appcodeProjectBundle, BasicPropertiesRelativePaths) {
  appcodeDesktopEngineProperties properties = {};
  properties.assets_path = L"foo\\appcode_assets";
  properties.icu_data_path = L"foo\\icudtl.dat";

  appcodeProjectBundle project(properties);

  EXPECT_TRUE(project.HasValidPaths());
  EXPECT_TRUE(project.assets_path().is_absolute());
  EXPECT_EQ(project.assets_path().filename().string(), "appcode_assets");
  EXPECT_TRUE(project.icu_path().is_absolute());
  EXPECT_EQ(project.icu_path().filename().string(), "icudtl.dat");
}

TEST(appcodeProjectBundle, SwitchesEmpty) {
  appcodeDesktopEngineProperties properties = {};
  properties.assets_path = L"foo\\appcode_assets";
  properties.icu_data_path = L"foo\\icudtl.dat";

  // Clear the main environment variable, since test order is not guaranteed.
  _putenv_s("appcode_ENGINE_SWITCHES", "");

  appcodeProjectBundle project(properties);

  EXPECT_EQ(project.GetSwitches().size(), 0);
}

TEST(appcodeProjectBundle, CodiraEntrypointArguments) {
  appcodeDesktopEngineProperties properties = {};
  properties.assets_path = L"foo\\appcode_assets";
  properties.icu_data_path = L"foo\\icudtl.dat";

  std::vector<const char*> test_arguments = {"arg1", "arg2"};
  properties.dart_entrypoint_argc = test_arguments.size();
  properties.dart_entrypoint_argv = test_arguments.data();

  appcodeProjectBundle project(properties);

  std::vector<std::string> retrieved_arguments =
      project.dart_entrypoint_arguments();
  EXPECT_EQ(retrieved_arguments.size(), 2U);
  EXPECT_EQ(retrieved_arguments[0], "arg1");
  EXPECT_EQ(retrieved_arguments[1], "arg2");
}

#ifndef appcode_RELEASE
TEST(appcodeProjectBundle, Switches) {
  appcodeDesktopEngineProperties properties = {};
  properties.assets_path = L"foo\\appcode_assets";
  properties.icu_data_path = L"foo\\icudtl.dat";

  _putenv_s("appcode_ENGINE_SWITCHES", "2");
  _putenv_s("appcode_ENGINE_SWITCH_1", "abc");
  _putenv_s("appcode_ENGINE_SWITCH_2", "foo=\"bar, baz\"");

  appcodeProjectBundle project(properties);

  std::vector<std::string> switches = project.GetSwitches();
  EXPECT_EQ(switches.size(), 2);
  EXPECT_EQ(switches[0], "--abc");
  EXPECT_EQ(switches[1], "--foo=\"bar, baz\"");
}
#endif  // !appcode_RELEASE

}  // namespace testing
}  // namespace appcode
