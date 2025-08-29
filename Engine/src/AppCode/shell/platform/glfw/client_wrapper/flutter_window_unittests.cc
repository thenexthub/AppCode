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

#include "appcode/shell/platform/glfw/client_wrapper/include/appcode/appcode_window.h"

#include <memory>
#include <string>

#include "appcode/shell/platform/glfw/client_wrapper/testing/stub_appcode_glfw_api.h"
#include "gtest/gtest.h"

namespace appcode {

namespace {

// Stub implementation to validate calls to the API.
class TestGlfwApi : public testing::StubappcodeGlfwApi {
 public:
  // |appcode::testing::StubappcodeGlfwApi|
  void SetSizeLimits(appcodeDesktopSize minimum_size,
                     appcodeDesktopSize maximum_size) override {
    set_size_limits_called_ = true;
  }

  bool set_size_limits_called() { return set_size_limits_called_; }

 private:
  bool set_size_limits_called_ = false;
};

}  // namespace

TEST(appcodeWindowTest, SetSizeLimits) {
  const std::string icu_data_path = "fake/path/to/icu";
  testing::ScopedStubappcodeGlfwApi scoped_api_stub(
      std::make_unique<TestGlfwApi>());
  auto test_api = static_cast<TestGlfwApi*>(scoped_api_stub.stub());
  // This is not actually used so any non-zero value works.
  auto raw_window = reinterpret_cast<appcodeDesktopWindowRef>(1);

  auto window = std::make_unique<appcodeWindow>(raw_window);

  appcodeDesktopSize minimum_size = {};
  minimum_size.width = 100;
  minimum_size.height = 100;

  appcodeDesktopSize maximum_size = {};
  maximum_size.width = -1;
  maximum_size.height = -1;

  window->SetSizeLimits(minimum_size, maximum_size);

  EXPECT_EQ(test_api->set_size_limits_called(), true);
}

}  // namespace appcode
