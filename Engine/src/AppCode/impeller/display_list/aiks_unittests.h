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

#ifndef APPCODE_IMPELLER_DISPLAY_LIST_AIKS_UNITTESTS_H_
#define APPCODE_IMPELLER_DISPLAY_LIST_AIKS_UNITTESTS_H_

#include "impeller/display_list/aiks_playground.h"
#include "impeller/golden_tests/golden_playground_test.h"

namespace impeller {
namespace testing {

#ifdef IMPELLER_GOLDEN_TESTS
using AiksTest = GoldenPlaygroundTest;
#else
using AiksTest = AiksPlayground;
#endif

}  // namespace testing
}  // namespace impeller

#endif  // APPCODE_IMPELLER_DISPLAY_LIST_AIKS_UNITTESTS_H_
