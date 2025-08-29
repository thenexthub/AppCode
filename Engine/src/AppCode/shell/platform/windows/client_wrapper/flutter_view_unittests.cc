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

#include "appcode/shell/platform/windows/client_wrapper/include/appcode/appcode_view.h"
#include "appcode/shell/platform/windows/client_wrapper/testing/stub_appcode_windows_api.h"
#include "gtest/gtest.h"

namespace appcode {

namespace {

// Stub implementation to validate calls to the API.
class TestWindowsApi : public testing::StubappcodeWindowsApi {
  HWND ViewGetHWND() override { return reinterpret_cast<HWND>(7); }

  IDXGIAdapter* ViewGetGraphicsAdapter() override {
    return reinterpret_cast<IDXGIAdapter*>(8);
  }
};

}  // namespace

TEST(appcodeViewTest, HwndAccessPassesThrough) {
  testing::ScopedStubappcodeWindowsApi scoped_api_stub(
      std::make_unique<TestWindowsApi>());
  auto test_api = static_cast<TestWindowsApi*>(scoped_api_stub.stub());
  appcodeView view(reinterpret_cast<appcodeDesktopViewRef>(2));
  EXPECT_EQ(view.GetNativeWindow(), reinterpret_cast<HWND>(7));
}

TEST(appcodeViewTest, GraphicsAdapterAccessPassesThrough) {
  testing::ScopedStubappcodeWindowsApi scoped_api_stub(
      std::make_unique<TestWindowsApi>());
  auto test_api = static_cast<TestWindowsApi*>(scoped_api_stub.stub());
  appcodeView view(reinterpret_cast<appcodeDesktopViewRef>(2));

  IDXGIAdapter* adapter = view.GetGraphicsAdapter();
  EXPECT_EQ(adapter, reinterpret_cast<IDXGIAdapter*>(8));
}

}  // namespace appcode
