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

#include "appcode/shell/platform/android/surface/android_native_window.h"

namespace appcode {

AndroidNativeWindow::AndroidNativeWindow(Handle window, bool is_fake_window)
    : window_(window), is_fake_window_(is_fake_window) {}

AndroidNativeWindow::AndroidNativeWindow(Handle window)
    : AndroidNativeWindow(window, /*is_fake_window=*/false) {}

AndroidNativeWindow::~AndroidNativeWindow() {
  if (window_ != nullptr) {
#if FML_OS_ANDROID
    ANativeWindow_release(window_);
    window_ = nullptr;
#endif  // FML_OS_ANDROID
  }
}

bool AndroidNativeWindow::IsValid() const {
  return window_ != nullptr;
}

AndroidNativeWindow::Handle AndroidNativeWindow::handle() const {
  return window_;
}

SkISize AndroidNativeWindow::GetSize() const {
#if FML_OS_ANDROID
  return window_ == nullptr ? SkISize::Make(0, 0)
                            : SkISize::Make(ANativeWindow_getWidth(window_),
                                            ANativeWindow_getHeight(window_));
#else   // FML_OS_ANDROID
  return SkISize::Make(0, 0);
#endif  // FML_OS_ANDROID
}

}  // namespace appcode
