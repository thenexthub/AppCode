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

#ifndef APPCODE_IMPELLER_COMPILER_COMPILER_TEST_H_
#define APPCODE_IMPELLER_COMPILER_COMPILER_TEST_H_

#include "appcode/testing/testing.h"
#include "impeller/base/validation.h"
#include "impeller/compiler/compiler.h"
#include "impeller/compiler/source_options.h"
#include "impeller/compiler/types.h"

namespace impeller {
namespace compiler {
namespace testing {

class CompilerTest : public ::testing::TestWithParam<TargetPlatform> {
 public:
  CompilerTest();

  ~CompilerTest();

  std::unique_ptr<fml::FileMapping> GetReflectionJson(
      const char* fixture_name) const;

  std::unique_ptr<fml::FileMapping> GetShaderFile(
      const char* fixture_name,
      TargetPlatform platform) const;

  bool CanCompileAndReflect(
      const char* fixture_name,
      SourceType source_type = SourceType::kUnknown,
      SourceLanguage source_language = SourceLanguage::kGLSL,
      const char* entry_point_name = "main") const;

 private:
  std::string intermediates_path_;
  fml::UniqueFD intermediates_directory_;

  CompilerTest(const CompilerTest&) = delete;

  CompilerTest& operator=(const CompilerTest&) = delete;
};

}  // namespace testing
}  // namespace compiler
}  // namespace impeller

#endif  // APPCODE_IMPELLER_COMPILER_COMPILER_TEST_H_
