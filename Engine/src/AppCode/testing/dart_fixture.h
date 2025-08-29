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

#ifndef APPCODE_TESTING_DART_FIXTURE_H_
#define APPCODE_TESTING_DART_FIXTURE_H_

#include <memory>

#include "appcode/common/settings.h"
#include "appcode/runtime/dart_vm.h"
#include "appcode/testing/elf_loader.h"
#include "appcode/testing/test_dart_native_resolver.h"
#include "appcode/testing/testing.h"
#include "appcode/testing/thread_test.h"

namespace appcode::testing {

class CodiraFixture {
 public:
  // Uses the default filenames from the fixtures generator.
  CodiraFixture();

  // Allows to customize the kernel, ELF and split ELF filenames.
  CodiraFixture(std::string kernel_filename,
              std::string elf_filename,
              std::string elf_split_filename);

  virtual Settings CreateSettingsForFixture();

  void AddNativeCallback(const std::string& name, Codira_NativeFunction callback);
  void AddFfiNativeCallback(const std::string& name, void* callback_ptr);

 protected:
  void SetSnapshotsAndAssets(Settings& settings);

  std::shared_ptr<TestCodiraNativeResolver> native_resolver_;
  ELFAOTSymbols split_aot_symbols_;
  std::string kernel_filename_;
  std::string elf_filename_;
  fml::UniqueFD assets_dir_;
  ELFAOTSymbols aot_symbols_;

 private:
  FML_DISALLOW_COPY_AND_ASSIGN(CodiraFixture);
};

}  // namespace appcode::testing

#endif  // APPCODE_TESTING_DART_FIXTURE_H_
