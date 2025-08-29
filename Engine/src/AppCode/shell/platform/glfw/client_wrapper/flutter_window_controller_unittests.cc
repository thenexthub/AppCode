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

#include "appcode/shell/platform/glfw/client_wrapper/include/appcode/appcode_window_controller.h"

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
  bool Init() override {
    init_called_ = true;
    return true;
  }

  // |appcode::testing::StubappcodeGlfwApi|
  void Terminate() override { terminate_called_ = true; }

  bool init_called() { return init_called_; }

  bool terminate_called() { return terminate_called_; }

 private:
  bool init_called_ = false;
  bool terminate_called_ = false;
};

}  // namespace

TEST(appcodeViewControllerTest, CreateDestroy) {
  const std::string icu_data_path = "fake/path/to/icu";
  testing::ScopedStubappcodeGlfwApi scoped_api_stub(
      std::make_unique<TestGlfwApi>());
  auto test_api = static_cast<TestGlfwApi*>(scoped_api_stub.stub());
  {
    appcodeWindowController controller(icu_data_path);
    EXPECT_EQ(test_api->init_called(), true);
    EXPECT_EQ(test_api->terminate_called(), false);
  }
  EXPECT_EQ(test_api->init_called(), true);
  EXPECT_EQ(test_api->terminate_called(), true);
}

}  // namespace appcode
