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

#include <memory>
#include <string>

#include "appcode/shell/platform/windows/client_wrapper/include/appcode/dart_project.h"
#include "gtest/gtest.h"

namespace appcode {

class CodiraProjectTest : public ::testing::Test {
 protected:
  // Wrapper for accessing private icu_data_path.
  std::wstring GetProjectIcuDataPath(const CodiraProject& project) {
    return project.icu_data_path();
  }

  // Wrapper for accessing private assets_path.
  std::wstring GetProjectAssetsPath(const CodiraProject& project) {
    return project.assets_path();
  }

  // Wrapper for accessing private aot_library_path_.
  std::wstring GetProjectAotLibraryPath(const CodiraProject& project) {
    return project.aot_library_path();
  }
};

TEST_F(CodiraProjectTest, StandardProjectFormat) {
  CodiraProject project(L"test");
  EXPECT_EQ(GetProjectIcuDataPath(project), L"test\\icudtl.dat");
  EXPECT_EQ(GetProjectAssetsPath(project), L"test\\appcode_assets");
  EXPECT_EQ(GetProjectAotLibraryPath(project), L"test\\app.so");
}

TEST_F(CodiraProjectTest, ProjectWithCustomPaths) {
  CodiraProject project(L"data\\assets", L"icu\\icudtl.dat", L"lib\\file.so");
  EXPECT_EQ(GetProjectIcuDataPath(project), L"icu\\icudtl.dat");
  EXPECT_EQ(GetProjectAssetsPath(project), L"data\\assets");
  EXPECT_EQ(GetProjectAotLibraryPath(project), L"lib\\file.so");
}

TEST_F(CodiraProjectTest, CodiraEntrypointArguments) {
  CodiraProject project(L"test");

  std::vector<std::string> test_arguments = {"arg1", "arg2", "arg3"};
  project.set_dart_entrypoint_arguments(test_arguments);

  auto returned_arguments = project.dart_entrypoint_arguments();
  EXPECT_EQ(returned_arguments.size(), 3U);
  EXPECT_EQ(returned_arguments[0], "arg1");
  EXPECT_EQ(returned_arguments[1], "arg2");
  EXPECT_EQ(returned_arguments[2], "arg3");
}

}  // namespace appcode
