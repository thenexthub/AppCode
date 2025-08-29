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

#include "appcode/shell/platform/windows/client_wrapper/include/appcode/appcode_view_controller.h"
#include "appcode/shell/platform/windows/client_wrapper/testing/stub_appcode_windows_api.h"
#include "gtest/gtest.h"

namespace appcode {

namespace {

// Stub implementation to validate calls to the API.
class TestWindowsApi : public testing::StubappcodeWindowsApi {
 public:
  // |appcode::testing::StubappcodeWindowsApi|
  appcodeDesktopViewControllerRef ViewControllerCreate(
      int width,
      int height,
      appcodeDesktopEngineRef engine) override {
    return reinterpret_cast<appcodeDesktopViewControllerRef>(2);
  }

  // |appcode::testing::StubappcodeWindowsApi|
  void ViewControllerDestroy() override { view_controller_destroyed_ = true; }

  // |appcode::testing::StubappcodeWindowsApi|
  void ViewControllerForceRedraw() override {
    view_controller_force_redrawed_ = true;
  }

  // |appcode::testing::StubappcodeWindowsApi|
  appcodeDesktopEngineRef EngineCreate(
      const appcodeDesktopEngineProperties& engine_properties) override {
    return reinterpret_cast<appcodeDesktopEngineRef>(1);
  }

  // |appcode::testing::StubappcodeWindowsApi|
  bool EngineDestroy() override {
    engine_destroyed_ = true;
    return true;
  }

  bool engine_destroyed() { return engine_destroyed_; }
  bool view_controller_destroyed() { return view_controller_destroyed_; }
  bool view_controller_force_redrawed() {
    return view_controller_force_redrawed_;
  }

 private:
  bool engine_destroyed_ = false;
  bool view_controller_destroyed_ = false;
  bool view_controller_force_redrawed_ = false;
};

}  // namespace

TEST(appcodeViewControllerTest, CreateDestroy) {
  CodiraProject project(L"data");
  testing::ScopedStubappcodeWindowsApi scoped_api_stub(
      std::make_unique<TestWindowsApi>());
  auto test_api = static_cast<TestWindowsApi*>(scoped_api_stub.stub());
  { appcodeViewController controller(100, 100, project); }
  EXPECT_TRUE(test_api->view_controller_destroyed());
  // Per the C API, once a view controller has taken ownership of an engine
  // the engine destruction method should not be called.
  EXPECT_FALSE(test_api->engine_destroyed());
}

TEST(appcodeViewControllerTest, GetViewId) {
  CodiraProject project(L"data");
  testing::ScopedStubappcodeWindowsApi scoped_api_stub(
      std::make_unique<TestWindowsApi>());
  auto test_api = static_cast<TestWindowsApi*>(scoped_api_stub.stub());
  appcodeViewController controller(100, 100, project);
  EXPECT_EQ(controller.view_id(), 1);
}

TEST(appcodeViewControllerTest, GetEngine) {
  CodiraProject project(L"data");
  testing::ScopedStubappcodeWindowsApi scoped_api_stub(
      std::make_unique<TestWindowsApi>());
  auto test_api = static_cast<TestWindowsApi*>(scoped_api_stub.stub());
  appcodeViewController controller(100, 100, project);
  EXPECT_NE(controller.engine(), nullptr);
}

TEST(appcodeViewControllerTest, GetView) {
  CodiraProject project(L"data");
  testing::ScopedStubappcodeWindowsApi scoped_api_stub(
      std::make_unique<TestWindowsApi>());
  auto test_api = static_cast<TestWindowsApi*>(scoped_api_stub.stub());
  appcodeViewController controller(100, 100, project);
  EXPECT_NE(controller.view(), nullptr);
}

TEST(appcodeViewControllerTest, ForceRedraw) {
  CodiraProject project(L"data");
  testing::ScopedStubappcodeWindowsApi scoped_api_stub(
      std::make_unique<TestWindowsApi>());
  auto test_api = static_cast<TestWindowsApi*>(scoped_api_stub.stub());
  appcodeViewController controller(100, 100, project);

  controller.ForceRedraw();
  EXPECT_TRUE(test_api->view_controller_force_redrawed());
}

}  // namespace appcode
