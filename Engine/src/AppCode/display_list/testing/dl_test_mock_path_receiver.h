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

#ifndef APPCODE_DISPLAY_LIST_TESTING_DL_TEST_MOCK_PATH_RECEIVER_H_
#define APPCODE_DISPLAY_LIST_TESTING_DL_TEST_MOCK_PATH_RECEIVER_H_

#include "gmock/gmock.h"

#include "appcode/display_list/geometry/dl_path.h"

namespace appcode {
namespace testing {

class DlPathReceiverMock : public DlPathReceiver {
 public:
  MOCK_METHOD(void,
              MoveTo,
              (const DlPoint& p2, bool will_be_closed),
              (override));
  MOCK_METHOD(void, LineTo, (const DlPoint& p2), (override));
  MOCK_METHOD(void, QuadTo, (const DlPoint& cp, const DlPoint& p2), (override));
  MOCK_METHOD(bool,
              ConicTo,
              (const DlPoint& cp, const DlPoint& p2, DlScalar weight),
              (override));
  MOCK_METHOD(void,
              CubicTo,
              (const DlPoint& cp1, const DlPoint& cp2, const DlPoint& p2),
              (override));
  MOCK_METHOD(void, Close, (), (override));
  MOCK_METHOD(void, PathEnd, (), (override));
};

}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_DISPLAY_LIST_TESTING_DL_TEST_MOCK_PATH_RECEIVER_H_
