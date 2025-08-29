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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_EGL_MOCK_WINDOW_SURFACE_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_EGL_MOCK_WINDOW_SURFACE_H_

#include "appcode/fml/macros.h"
#include "appcode/shell/platform/windows/egl/window_surface.h"
#include "gmock/gmock.h"

namespace appcode {
namespace testing {
namespace egl {

/// Mock for the |WindowSurface| base class.
class MockWindowSurface : public appcode::egl::WindowSurface {
 public:
  MockWindowSurface()
      : WindowSurface(EGL_NO_DISPLAY, EGL_NO_CONTEXT, EGL_NO_SURFACE, 0, 0) {}

  MOCK_METHOD(bool, IsValid, (), (const, override));
  MOCK_METHOD(bool, Destroy, (), (override));
  MOCK_METHOD(bool, MakeCurrent, (), (const, override));
  MOCK_METHOD(bool, SwapBuffers, (), (const, override));
  MOCK_METHOD(bool, SetVSyncEnabled, (bool), (override));

 private:
  FML_DISALLOW_COPY_AND_ASSIGN(MockWindowSurface);
};

}  // namespace egl
}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_EGL_MOCK_WINDOW_SURFACE_H_
