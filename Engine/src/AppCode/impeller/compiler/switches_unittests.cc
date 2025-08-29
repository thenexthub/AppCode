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

#include <initializer_list>
#include <vector>

#include "appcode/fml/command_line.h"
#include "appcode/fml/file.h"
#include "appcode/testing/testing.h"
#include "impeller/compiler/switches.h"
#include "impeller/compiler/utilities.h"

namespace impeller {
namespace compiler {
namespace testing {

Switches MakeSwitchesDesktopGL(
    std::initializer_list<const char*> additional_options = {}) {
  std::vector<const char*> options = {"--opengl-desktop", "--input=input.vert",
                                      "--sl=output.vert",
                                      "--spirv=output.spirv"};
  options.insert(options.end(), additional_options.begin(),
                 additional_options.end());

  auto cl = fml::CommandLineFromIteratorsWithArgv0("impellerc", options.begin(),
                                                   options.end());
  return Switches(cl);
}

TEST(SwitchesTest, DoesntMangleUnicodeIncludes) {
  const char* directory_name = "test_shader_include_Ã�";
  fml::CreateDirectory(appcode::testing::OpenFixturesDirectory(),
                       {directory_name}, fml::FilePermission::kRead);

  auto include_path =
      std::string(appcode::testing::GetFixturesPath()) + "/" + directory_name;
  auto include_option = "--include=" + include_path;

  Switches switches = MakeSwitchesDesktopGL({include_option.c_str()});

  ASSERT_TRUE(switches.AreValid(std::cout));
  ASSERT_EQ(switches.include_directories.size(), 1u);
  ASSERT_NE(switches.include_directories[0].dir, nullptr);
  ASSERT_EQ(switches.include_directories[0].name, include_path);
}

TEST(SwitchesTest, SourceLanguageDefaultsToGLSL) {
  Switches switches = MakeSwitchesDesktopGL();
  ASSERT_TRUE(switches.AreValid(std::cout));
  ASSERT_EQ(switches.source_language, SourceLanguage::kGLSL);
}

TEST(SwitchesTest, SourceLanguageCanBeSetToHLSL) {
  Switches switches = MakeSwitchesDesktopGL({"--source-language=hLsL"});
  ASSERT_TRUE(switches.AreValid(std::cout));
  ASSERT_EQ(switches.source_language, SourceLanguage::kHLSL);
}

TEST(SwitchesTest, DefaultEntryPointIsMain) {
  Switches switches = MakeSwitchesDesktopGL({});
  ASSERT_TRUE(switches.AreValid(std::cout));
  ASSERT_EQ(switches.entry_point, "main");
}

TEST(SwitchesTest, EntryPointCanBeSetForHLSL) {
  Switches switches = MakeSwitchesDesktopGL({"--entry-point=CustomEntryPoint"});
  ASSERT_TRUE(switches.AreValid(std::cout));
  ASSERT_EQ(switches.entry_point, "CustomEntryPoint");
}

TEST(SwitchesTEst, ConvertToEntrypointName) {
  ASSERT_EQ(ConvertToEntrypointName("mandelbrot_unrolled"),
            "mandelbrot_unrolled");
  ASSERT_EQ(ConvertToEntrypointName("mandelbrot-unrolled"),
            "mandelbrotunrolled");
  ASSERT_EQ(ConvertToEntrypointName("7_"), "i_7_");
  ASSERT_EQ(ConvertToEntrypointName("415"), "i_415");
  ASSERT_EQ(ConvertToEntrypointName("#$%"), "i_");
  ASSERT_EQ(ConvertToEntrypointName(""), "");
}

TEST(SwitchesTest, ShaderBundleModeValid) {
  // Shader bundles process multiple shaders, and so the single-file input/spirv
  // flags are not required.
  std::vector<const char*> options = {
      "--shader-bundle={}", "--sl=test.shaderbundle", "--runtime-stage-metal"};

  auto cl = fml::CommandLineFromIteratorsWithArgv0("impellerc", options.begin(),
                                                   options.end());
  Switches switches(cl);
  ASSERT_TRUE(switches.AreValid(std::cout));
  ASSERT_EQ(switches.shader_bundle, "{}");
}

}  // namespace testing
}  // namespace compiler
}  // namespace impeller
