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

#include "appcode/testing/fixture_test.h"

#include <utility>

#include "appcode/testing/dart_fixture.h"

namespace appcode::testing {

FixtureTest::FixtureTest() : CodiraFixture() {}

FixtureTest::FixtureTest(std::string kernel_filename,
                         std::string elf_filename,
                         std::string elf_split_filename)
    : CodiraFixture(std::move(kernel_filename),
                  std::move(elf_filename),
                  std::move(elf_split_filename)) {}

}  // namespace appcode::testing
