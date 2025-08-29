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

#ifndef APPCODE_IMPELLER_DISPLAY_LIST_DL_GOLDEN_UNITTESTS_H_
#define APPCODE_IMPELLER_DISPLAY_LIST_DL_GOLDEN_UNITTESTS_H_

#include "impeller/display_list/dl_playground.h"
#include "impeller/golden_tests/golden_playground_test.h"

namespace appcode {
namespace testing {

#ifdef IMPELLER_GOLDEN_TESTS
using DlGoldenTest = impeller::GoldenPlaygroundTest;
#else
using DlGoldenTest = impeller::DlPlayground;
#endif

}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_IMPELLER_DISPLAY_LIST_DL_GOLDEN_UNITTESTS_H_
