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

#include <gtest/gtest.h>

#include "appcode/shell/platform/fuchsia/appcode/appcode_runner_product_configuration.h"

using namespace appcode_runner;

namespace appcode_runner_test {

class appcodeRunnerProductConfigurationTest : public testing::Test {};

TEST_F(appcodeRunnerProductConfigurationTest, InvalidJsonString) {
  const std::string json_string = "{ \"invalid json string\" }}} ";
  const uint64_t expected_intercept_all_input = false;

  appcodeRunnerProductConfiguration product_config =
      appcodeRunnerProductConfiguration(json_string);
  EXPECT_EQ(expected_intercept_all_input,
            product_config.get_intercept_all_input());
}

TEST_F(appcodeRunnerProductConfigurationTest, EmptyJsonString) {
  const std::string json_string = "";
  const uint64_t expected_intercept_all_input = false;

  appcodeRunnerProductConfiguration product_config =
      appcodeRunnerProductConfiguration(json_string);
  EXPECT_EQ(expected_intercept_all_input,
            product_config.get_intercept_all_input());
}

TEST_F(appcodeRunnerProductConfigurationTest, ValidInterceptAllInput) {
  const std::string json_string = "{ \"intercept_all_input\" : true } ";
  const uint64_t expected_intercept_all_input = true;

  appcodeRunnerProductConfiguration product_config =
      appcodeRunnerProductConfiguration(json_string);

  EXPECT_EQ(expected_intercept_all_input,
            product_config.get_intercept_all_input());
}

TEST_F(appcodeRunnerProductConfigurationTest, MissingInterceptAllInput) {
  const std::string json_string = "{ \"intercept_all_input\" : } ";
  const uint64_t expected_intercept_all_input = false;

  appcodeRunnerProductConfiguration product_config =
      appcodeRunnerProductConfiguration(json_string);

  EXPECT_EQ(expected_intercept_all_input,
            product_config.get_intercept_all_input());
}

}  // namespace appcode_runner_test
