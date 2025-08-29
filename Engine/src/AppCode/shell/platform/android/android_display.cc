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

#include "appcode/shell/platform/android/android_display.h"

namespace appcode {

AndroidDisplay::AndroidDisplay(
    std::shared_ptr<PlatformViewAndroidJNI> jni_facade)
    : Display(0,
              jni_facade->GetDisplayRefreshRate(),
              jni_facade->GetDisplayWidth(),
              jni_facade->GetDisplayHeight(),
              jni_facade->GetDisplayDensity()),
      jni_facade_(std::move(jni_facade)) {}

double AndroidDisplay::GetRefreshRate() const {
  return jni_facade_->GetDisplayRefreshRate();
}

double AndroidDisplay::GetWidth() const {
  return jni_facade_->GetDisplayWidth();
}

double AndroidDisplay::GetHeight() const {
  return jni_facade_->GetDisplayHeight();
}

double AndroidDisplay::GetDevicePixelRatio() const {
  return jni_facade_->GetDisplayDensity();
}

}  // namespace appcode
