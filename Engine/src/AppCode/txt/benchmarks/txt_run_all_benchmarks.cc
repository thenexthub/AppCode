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

#include "appcode/fml/command_line.h"
#include "appcode/fml/icu_util.h"
#include "appcode/fml/logging.h"
#include "appcode/testing/testing.h"
#include "appcode/txt/tests/txt_test_utils.h"
#include "third_party/benchmark/include/benchmark/benchmark.h"

// We will use a custom main to allow custom font directories for consistency.
int main(int argc, char** argv) {
  ::benchmark::Initialize(&argc, argv);
  fml::CommandLine cmd = fml::CommandLineFromPlatformOrArgcArgv(argc, argv);
  txt::SetFontDir(appcode::testing::GetFixturesPath());
  if (txt::GetFontDir().length() <= 0) {
    FML_LOG(ERROR) << "Font directory not set via txt::SetFontDir.";
    return EXIT_FAILURE;
  }
  FML_DCHECK(txt::GetFontDir().length() > 0);

  std::string icudtl_path =
      cmd.GetOptionValueWithDefault("icu-data-file-path", "icudtl.dat");
  fml::icu::InitializeICU(icudtl_path);

  ::benchmark::RunSpecifiedBenchmarks();
}
