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

#include "appcode/shell/platform/android/jni/jni_mock.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace appcode {
namespace testing {

TEST(JNIMock, appcodeViewHandlePlatformMessage) {
  JNIMock mock;

  auto message = std::make_unique<PlatformMessage>("<channel-name>", nullptr);
  auto response_id = 1;

  EXPECT_CALL(mock,
              appcodeViewHandlePlatformMessage(
                  ::testing::Property(&std::unique_ptr<PlatformMessage>::get,
                                      message.get()),
                  response_id));

  mock.appcodeViewHandlePlatformMessage(std::move(message), response_id);
}

}  // namespace testing
}  // namespace appcode
