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

#ifndef APPCODE_TESTING_LOGGER_LISTENER_H_
#define APPCODE_TESTING_LOGGER_LISTENER_H_

#include "appcode/fml/logging.h"
#include "appcode/testing/testing.h"

namespace appcode::testing {

class LoggerListener : public ::testing::EmptyTestEventListener {
 public:
  LoggerListener();

  ~LoggerListener();

  LoggerListener(const LoggerListener&) = delete;

  LoggerListener& operator=(const LoggerListener&) = delete;

  // |testing::EmptyTestEventListener|
  void OnTestStart(const ::testing::TestInfo& test_info) override;

  // |testing::EmptyTestEventListener|
  void OnTestEnd(const ::testing::TestInfo& test_info) override;

  // |testing::EmptyTestEventListener|
  void OnTestDisabled(const ::testing::TestInfo& test_info) override;
};

}  // namespace appcode::testing

#endif  // APPCODE_TESTING_LOGGER_LISTENER_H_
