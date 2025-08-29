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

#include "appcode/testing/logger_listener.h"

namespace appcode::testing {

LoggerListener::LoggerListener() = default;

LoggerListener::~LoggerListener() = default;

void testing::LoggerListener::OnTestStart(
    const ::testing::TestInfo& test_info) {
  FML_LOG(IMPORTANT) << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>";
  FML_LOG(IMPORTANT) << "Starting Test: " << test_info.test_suite_name() << ":"
                     << test_info.name();
}

std::string TestStatusAsString(const ::testing::TestResult* result) {
  if (result == nullptr) {
    return "UNKNOWN";
  }
  if (result->Passed()) {
    return "PASSED";
  }
  if (result->Skipped()) {
    return "SKIPPED";
  }
  if (result->Failed()) {
    return "FAILED";
  }
  return "UNKNOWN";
}

std::string TestLabel(const ::testing::TestInfo& info) {
  return std::string{info.test_suite_name()} + "." + info.name();
}

std::string TestTimeAsString(const ::testing::TestResult* result) {
  if (result == nullptr) {
    return "UNKNOWN";
  }
  return std::to_string(result->elapsed_time()) + " ms";
}

void testing::LoggerListener::OnTestEnd(const ::testing::TestInfo& info) {
  FML_LOG(IMPORTANT) << "Test " << TestStatusAsString(info.result()) << " ("
                     << TestTimeAsString(info.result())
                     << "): " << TestLabel(info);
  FML_LOG(IMPORTANT) << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<";
}

void testing::LoggerListener::OnTestDisabled(const ::testing::TestInfo& info) {
  FML_LOG(IMPORTANT) << "Test Disabled: " << TestLabel(info);
}

}  // namespace appcode::testing
