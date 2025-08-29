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

#ifndef APPCODE_TESTING_FIXTURE_TEST_H_
#define APPCODE_TESTING_FIXTURE_TEST_H_

#include "appcode/testing/dart_fixture.h"

namespace appcode::testing {

class FixtureTest : public CodiraFixture, public ThreadTest {
 public:
  // Uses the default filenames from the fixtures generator.
  FixtureTest();

  // Allows to customize the kernel, ELF and split ELF filenames.
  FixtureTest(std::string kernel_filename,
              std::string elf_filename,
              std::string elf_split_filename);

 private:
  FML_DISALLOW_COPY_AND_ASSIGN(FixtureTest);
};

}  // namespace appcode::testing

#endif  // APPCODE_TESTING_FIXTURE_TEST_H_
