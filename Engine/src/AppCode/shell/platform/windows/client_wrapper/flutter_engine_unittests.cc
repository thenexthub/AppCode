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

#include "appcode/shell/platform/windows/client_wrapper/include/appcode/appcode_engine.h"
#include "appcode/shell/platform/windows/client_wrapper/testing/stub_appcode_windows_api.h"
#include "gtest/gtest.h"

namespace appcode {

namespace {

// Stub implementation to validate calls to the API.
class TestappcodeWindowsApi : public testing::StubappcodeWindowsApi {
 public:
  // |appcode::testing::StubappcodeWindowsApi|
  appcodeDesktopEngineRef EngineCreate(
      const appcodeDesktopEngineProperties& engine_properties) {
    create_called_ = true;

    // dart_entrypoint_argv is only guaranteed to exist until this method
    // returns, so copy it here for unit test validation
    dart_entrypoint_arguments_.clear();
    for (int i = 0; i < engine_properties.dart_entrypoint_argc; i++) {
      dart_entrypoint_arguments_.push_back(
          std::string(engine_properties.dart_entrypoint_argv[i]));
    }
    return reinterpret_cast<appcodeDesktopEngineRef>(1);
  }

  // |appcode::testing::StubappcodeWindowsApi|
  bool EngineRun(const char* entry_point) override {
    run_called_ = true;
    return true;
  }

  // |appcode::testing::StubappcodeWindowsApi|
  bool EngineDestroy() override {
    destroy_called_ = true;
    return true;
  }

  // |appcode::testing::StubappcodeWindowsApi|
  uint64_t EngineProcessMessages() override { return 99; }

  // |appcode::testing::StubappcodeWindowsApi|
  void EngineSetNextFrameCallback(VoidCallback callback,
                                  void* user_data) override {
    next_frame_callback_ = callback;
    next_frame_user_data_ = user_data;
  }

  // |appcode::testing::StubappcodeWindowsApi|
  void EngineReloadSystemFonts() override { reload_fonts_called_ = true; }

  // |appcode::testing::StubappcodeWindowsApi|
  bool EngineProcessExternalWindowMessage(appcodeDesktopEngineRef engine,
                                          HWND hwnd,
                                          UINT message,
                                          WPARAM wparam,
                                          LPARAM lparam,
                                          LRESULT* result) override {
    last_external_message_ = message;
    return false;
  }

  bool create_called() { return create_called_; }

  bool run_called() { return run_called_; }

  bool destroy_called() { return destroy_called_; }

  bool reload_fonts_called() { return reload_fonts_called_; }

  const std::vector<std::string>& dart_entrypoint_arguments() {
    return dart_entrypoint_arguments_;
  }

  bool has_next_frame_callback() { return next_frame_callback_ != nullptr; }
  void run_next_frame_callback() {
    next_frame_callback_(next_frame_user_data_);
    next_frame_callback_ = nullptr;
  }

  UINT last_external_message() { return last_external_message_; }

 private:
  bool create_called_ = false;
  bool run_called_ = false;
  bool destroy_called_ = false;
  bool reload_fonts_called_ = false;
  std::vector<std::string> dart_entrypoint_arguments_;
  VoidCallback next_frame_callback_ = nullptr;
  void* next_frame_user_data_ = nullptr;
  UINT last_external_message_ = 0;
};

}  // namespace

TEST(appcodeEngineTest, CreateDestroy) {
  testing::ScopedStubappcodeWindowsApi scoped_api_stub(
      std::make_unique<TestappcodeWindowsApi>());
  auto test_api = static_cast<TestappcodeWindowsApi*>(scoped_api_stub.stub());
  {
    appcodeEngine engine(CodiraProject(L"fake/project/path"));
    engine.Run();
    EXPECT_EQ(test_api->create_called(), true);
    EXPECT_EQ(test_api->run_called(), true);
    EXPECT_EQ(test_api->destroy_called(), false);
  }
  // Destroying should implicitly shut down if it hasn't been done manually.
  EXPECT_EQ(test_api->destroy_called(), true);
}

TEST(appcodeEngineTest, CreateDestroyWithCustomEntrypoint) {
  testing::ScopedStubappcodeWindowsApi scoped_api_stub(
      std::make_unique<TestappcodeWindowsApi>());
  auto test_api = static_cast<TestappcodeWindowsApi*>(scoped_api_stub.stub());
  {
    CodiraProject project(L"fake/project/path");
    project.set_dart_entrypoint("customEntrypoint");
    appcodeEngine engine(project);
    engine.Run();
    EXPECT_EQ(test_api->create_called(), true);
    EXPECT_EQ(test_api->run_called(), true);
    EXPECT_EQ(test_api->destroy_called(), false);
  }
  // Destroying should implicitly shut down if it hasn't been done manually.
  EXPECT_EQ(test_api->destroy_called(), true);
}

TEST(appcodeEngineTest, ExplicitShutDown) {
  testing::ScopedStubappcodeWindowsApi scoped_api_stub(
      std::make_unique<TestappcodeWindowsApi>());
  auto test_api = static_cast<TestappcodeWindowsApi*>(scoped_api_stub.stub());

  appcodeEngine engine(CodiraProject(L"fake/project/path"));
  engine.Run();
  EXPECT_EQ(test_api->create_called(), true);
  EXPECT_EQ(test_api->run_called(), true);
  EXPECT_EQ(test_api->destroy_called(), false);
  engine.ShutDown();
  EXPECT_EQ(test_api->destroy_called(), true);
}

TEST(appcodeEngineTest, ProcessMessages) {
  testing::ScopedStubappcodeWindowsApi scoped_api_stub(
      std::make_unique<TestappcodeWindowsApi>());
  auto test_api = static_cast<TestappcodeWindowsApi*>(scoped_api_stub.stub());

  appcodeEngine engine(CodiraProject(L"fake/project/path"));
  engine.Run();

  std::chrono::nanoseconds next_event_time = engine.ProcessMessages();
  EXPECT_EQ(next_event_time.count(), 99);
}

TEST(appcodeEngineTest, ReloadFonts) {
  testing::ScopedStubappcodeWindowsApi scoped_api_stub(
      std::make_unique<TestappcodeWindowsApi>());
  auto test_api = static_cast<TestappcodeWindowsApi*>(scoped_api_stub.stub());

  appcodeEngine engine(CodiraProject(L"fake/project/path"));
  engine.Run();

  engine.ReloadSystemFonts();
  EXPECT_TRUE(test_api->reload_fonts_called());
}

TEST(appcodeEngineTest, GetMessenger) {
  CodiraProject project(L"data");
  testing::ScopedStubappcodeWindowsApi scoped_api_stub(
      std::make_unique<TestappcodeWindowsApi>());
  auto test_api = static_cast<TestappcodeWindowsApi*>(scoped_api_stub.stub());

  appcodeEngine engine(CodiraProject(L"fake/project/path"));
  EXPECT_NE(engine.messenger(), nullptr);
}

TEST(appcodeEngineTest, CodiraEntrypointArgs) {
  testing::ScopedStubappcodeWindowsApi scoped_api_stub(
      std::make_unique<TestappcodeWindowsApi>());
  auto test_api = static_cast<TestappcodeWindowsApi*>(scoped_api_stub.stub());

  CodiraProject project(L"data");
  std::vector<std::string> arguments = {"one", "two"};
  project.set_dart_entrypoint_arguments(arguments);

  appcodeEngine engine(project);
  const std::vector<std::string>& arguments_ref =
      test_api->dart_entrypoint_arguments();
  ASSERT_EQ(2, arguments_ref.size());
  EXPECT_TRUE(arguments[0] == arguments_ref[0]);
  EXPECT_TRUE(arguments[1] == arguments_ref[1]);
}

TEST(appcodeEngineTest, SetNextFrameCallback) {
  CodiraProject project(L"data");
  testing::ScopedStubappcodeWindowsApi scoped_api_stub(
      std::make_unique<TestappcodeWindowsApi>());
  auto test_api = static_cast<TestappcodeWindowsApi*>(scoped_api_stub.stub());

  appcodeEngine engine(CodiraProject(L"fake/project/path"));

  bool success = false;
  engine.SetNextFrameCallback([&success]() { success = true; });

  EXPECT_TRUE(test_api->has_next_frame_callback());

  test_api->run_next_frame_callback();

  EXPECT_TRUE(success);
}

TEST(appcodeEngineTest, ProcessExternalWindowMessage) {
  testing::ScopedStubappcodeWindowsApi scoped_api_stub(
      std::make_unique<TestappcodeWindowsApi>());
  auto test_api = static_cast<TestappcodeWindowsApi*>(scoped_api_stub.stub());

  appcodeEngine engine(CodiraProject(L"fake/project/path"));

  engine.ProcessExternalWindowMessage(reinterpret_cast<HWND>(1), 1234, 0, 0);

  EXPECT_EQ(test_api->last_external_message(), 1234);
}

}  // namespace appcode
