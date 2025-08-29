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

#include <memory>
#include <string>

#include "appcode/shell/platform/glfw/client_wrapper/include/appcode/appcode_engine.h"
#include "appcode/shell/platform/glfw/client_wrapper/testing/stub_appcode_glfw_api.h"
#include "gtest/gtest.h"

namespace appcode {

namespace {

// Stub implementation to validate calls to the API.
class TestGlfwApi : public testing::StubappcodeGlfwApi {
 public:
  // |appcode::testing::StubappcodeGlfwApi|
  appcodeDesktopEngineRef RunEngine(
      const appcodeDesktopEngineProperties& properties) override {
    run_called_ = true;
    return reinterpret_cast<appcodeDesktopEngineRef>(1);
  }

  // |appcode::testing::StubappcodeGlfwApi|
  void RunEngineEventLoopWithTimeout(uint32_t millisecond_timeout) override {
    last_run_loop_timeout_ = millisecond_timeout;
  }

  // |appcode::testing::StubappcodeGlfwApi|
  bool ShutDownEngine() override {
    shut_down_called_ = true;
    return true;
  }

  bool run_called() { return run_called_; }

  bool shut_down_called() { return shut_down_called_; }

  uint32_t last_run_loop_timeout() { return last_run_loop_timeout_; }

 private:
  bool run_called_ = false;
  bool shut_down_called_ = false;
  uint32_t last_run_loop_timeout_ = 0;
};

}  // namespace

TEST(appcodeEngineTest, CreateDestroy) {
  const std::string icu_data_path = "fake/path/to/icu";
  const std::string assets_path = "fake/path/to/assets";
  testing::ScopedStubappcodeGlfwApi scoped_api_stub(
      std::make_unique<TestGlfwApi>());
  auto test_api = static_cast<TestGlfwApi*>(scoped_api_stub.stub());
  {
    appcodeEngine engine;
    engine.Start(icu_data_path, assets_path, {});
    EXPECT_EQ(test_api->run_called(), true);
    EXPECT_EQ(test_api->shut_down_called(), false);
  }
  // Destroying should implicitly shut down if it hasn't been done manually.
  EXPECT_EQ(test_api->shut_down_called(), true);
}

TEST(appcodeEngineTest, ExplicitShutDown) {
  const std::string icu_data_path = "fake/path/to/icu";
  const std::string assets_path = "fake/path/to/assets";
  testing::ScopedStubappcodeGlfwApi scoped_api_stub(
      std::make_unique<TestGlfwApi>());
  auto test_api = static_cast<TestGlfwApi*>(scoped_api_stub.stub());

  appcodeEngine engine;
  engine.Start(icu_data_path, assets_path, {});
  EXPECT_EQ(test_api->run_called(), true);
  EXPECT_EQ(test_api->shut_down_called(), false);
  engine.ShutDown();
  EXPECT_EQ(test_api->shut_down_called(), true);
}

TEST(appcodeEngineTest, RunloopTimeoutTranslation) {
  const std::string icu_data_path = "fake/path/to/icu";
  const std::string assets_path = "fake/path/to/assets";
  testing::ScopedStubappcodeGlfwApi scoped_api_stub(
      std::make_unique<TestGlfwApi>());
  auto test_api = static_cast<TestGlfwApi*>(scoped_api_stub.stub());

  appcodeEngine engine;
  engine.Start(icu_data_path, assets_path, {});

  engine.RunEventLoopWithTimeout(std::chrono::milliseconds(100));
  EXPECT_EQ(test_api->last_run_loop_timeout(), 100U);

  engine.RunEventLoopWithTimeout(std::chrono::milliseconds::max() -
                                 std::chrono::milliseconds(1));
  EXPECT_EQ(test_api->last_run_loop_timeout(), UINT32_MAX);

  engine.RunEventLoopWithTimeout(std::chrono::milliseconds::max());
  EXPECT_EQ(test_api->last_run_loop_timeout(), 0U);
}

}  // namespace appcode
