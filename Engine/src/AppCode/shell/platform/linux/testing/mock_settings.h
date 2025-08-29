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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_TESTING_MOCK_SETTINGS_H_
#define APPCODE_SHELL_PLATFORM_LINUX_TESTING_MOCK_SETTINGS_H_

#include "appcode/shell/platform/linux/fl_settings.h"

#include "gmock/gmock.h"

namespace appcode {
namespace testing {

// Mock for FlSettings.
class MockSettings {
 public:
  MockSettings();
  ~MockSettings();

  // This was an existing use of operator overloading. It's against our style
  // guide but enabling clang tidy on header files is a higher priority than
  // fixing this.
  // NOLINTNEXTLINE(google-explicit-constructor)
  operator FlSettings*();

  MOCK_METHOD(FlClockFormat,
              fl_settings_get_clock_format,
              (FlSettings * settings));

  MOCK_METHOD(FlColorScheme,
              fl_settings_get_color_scheme,
              (FlSettings * settings));

  MOCK_METHOD(bool, fl_settings_get_enable_animations, (FlSettings * settings));

  MOCK_METHOD(bool, fl_settings_get_high_contrast, (FlSettings * settings));

  MOCK_METHOD(gdouble,
              fl_settings_get_text_scaling_factor,
              (FlSettings * settings));

 private:
  FlSettings* instance_ = nullptr;
};

}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_LINUX_TESTING_MOCK_SETTINGS_H_
