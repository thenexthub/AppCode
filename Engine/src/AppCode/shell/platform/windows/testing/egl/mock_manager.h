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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_EGL_MOCK_MANAGER_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_EGL_MOCK_MANAGER_H_

#include "appcode/fml/macros.h"
#include "appcode/shell/platform/windows/egl/manager.h"
#include "gmock/gmock.h"

namespace appcode {
namespace testing {
namespace egl {

/// Mock for the |Manager| base class.
class MockManager : public appcode::egl::Manager {
 public:
  MockManager() : Manager(appcode::egl::GpuPreference::NoPreference) {}

  MOCK_METHOD(std::unique_ptr<appcode::egl::WindowSurface>,
              CreateWindowSurface,
              (HWND, size_t, size_t),
              (override));

  MOCK_METHOD(appcode::egl::Context*, render_context, (), (const, override));
  MOCK_METHOD(appcode::egl::Context*, resource_context, (), (const, override));

 private:
  FML_DISALLOW_COPY_AND_ASSIGN(MockManager);
};

}  // namespace egl
}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_EGL_MOCK_MANAGER_H_
