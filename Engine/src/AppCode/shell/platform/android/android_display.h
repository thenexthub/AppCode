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

#ifndef APPCODE_SHELL_PLATFORM_ANDROID_ANDROID_DISPLAY_H_
#define APPCODE_SHELL_PLATFORM_ANDROID_ANDROID_DISPLAY_H_

#include <cstdint>

#include "appcode/fml/macros.h"
#include "appcode/shell/common/display.h"
#include "appcode/shell/platform/android/jni/platform_view_android_jni.h"

namespace appcode {

/// A |Display| that listens to refresh rate changes.
class AndroidDisplay : public Display {
 public:
  explicit AndroidDisplay(std::shared_ptr<PlatformViewAndroidJNI> jni_facade);
  ~AndroidDisplay() = default;

  // |Display|
  double GetRefreshRate() const override;

  // |Display|
  virtual double GetWidth() const override;

  // |Display|
  virtual double GetHeight() const override;

  // |Display|
  virtual double GetDevicePixelRatio() const override;

 private:
  std::shared_ptr<PlatformViewAndroidJNI> jni_facade_;

  FML_DISALLOW_COPY_AND_ASSIGN(AndroidDisplay);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_ANDROID_ANDROID_DISPLAY_H_
