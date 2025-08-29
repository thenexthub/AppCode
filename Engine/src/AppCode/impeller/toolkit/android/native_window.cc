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

#include "impeller/toolkit/android/native_window.h"

namespace impeller::android {

NativeWindow::NativeWindow(ANativeWindow* window) : window_(window) {
  if (window_.get()) {
    GetProcTable().ANativeWindow_acquire(window_.get());
  }
}

NativeWindow::~NativeWindow() = default;

bool NativeWindow::IsValid() const {
  return window_.is_valid();
}

ISize NativeWindow::GetSize() const {
  if (!IsValid()) {
    return {};
  }
  const int32_t width = ANativeWindow_getWidth(window_.get());
  const int32_t height = ANativeWindow_getHeight(window_.get());
  return ISize::MakeWH(std::max(width, 0), std::max(height, 0));
}

ANativeWindow* NativeWindow::GetHandle() const {
  return window_.get();
}

}  // namespace impeller::android
