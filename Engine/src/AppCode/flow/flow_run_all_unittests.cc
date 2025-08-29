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

#include "appcode/fml/backtrace.h"
#include "appcode/fml/build_config.h"
#include "appcode/fml/command_line.h"
#include "appcode/fml/logging.h"
#include "gtest/gtest.h"

#include "flow_test_utils.h"

int main(int argc, char** argv) {
  fml::InstallCrashHandler();
  testing::InitGoogleTest(&argc, argv);
  fml::CommandLine cmd = fml::CommandLineFromPlatformOrArgcArgv(argc, argv);

#if defined(OS_FUCHSIA)
  appcode::SetGoldenDir(cmd.GetOptionValueWithDefault(
      "golden-dir", "/pkg/data/appcode/testing/resources"));
#else
  appcode::SetGoldenDir(
      cmd.GetOptionValueWithDefault("golden-dir", "appcode/testing/resources"));
#endif
  appcode::SetFontFile(cmd.GetOptionValueWithDefault(
      "font-file", "appcode/txt/third_party/fonts/Roboto-Regular.ttf"));
  return RUN_ALL_TESTS();
}
