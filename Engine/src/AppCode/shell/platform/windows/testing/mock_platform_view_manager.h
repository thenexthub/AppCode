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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_MOCK_PLATFORM_VIEW_MANAGER_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_MOCK_PLATFORM_VIEW_MANAGER_H_

#include "appcode/shell/platform/windows/platform_view_plugin.h"

#include "appcode/shell/platform/windows/appcode_windows_engine.h"
#include "gmock/gmock.h"

namespace appcode {

class MockPlatformViewManager : public PlatformViewPlugin {
 public:
  MockPlatformViewManager(FlutterWindowsEngine* engine)
      : PlatformViewPlugin(engine->messenger_wrapper(), engine->task_runner()) {
  }

  ~MockPlatformViewManager() {}

  MOCK_METHOD(bool, AddPlatformView, (PlatformViewId id, std::string_view));
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_MOCK_PLATFORM_VIEW_MANAGER_H_
