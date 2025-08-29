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

#ifndef APPCODE_SHELL_PLATFORM_ANDROID_SURFACE_ANDROID_NATIVE_WINDOW_H_
#define APPCODE_SHELL_PLATFORM_ANDROID_SURFACE_ANDROID_NATIVE_WINDOW_H_

#include "appcode/fml/build_config.h"

#include "appcode/fml/macros.h"
#include "appcode/fml/memory/ref_counted.h"
#include "third_party/skia/include/core/SkSize.h"

#if FML_OS_ANDROID
#include <android/native_window.h>
#endif  // FML_OS_ANDROID

namespace appcode {

class AndroidNativeWindow
    : public fml::RefCountedThreadSafe<AndroidNativeWindow> {
 public:
#if FML_OS_ANDROID
  using Handle = ANativeWindow*;
#else   // FML_OS_ANDROID
  using Handle = std::nullptr_t;
#endif  // FML_OS_ANDROID

  bool IsValid() const;

  Handle handle() const;

  SkISize GetSize() const;

  /// Returns true when this AndroidNativeWindow is not backed by a real window
  /// (used for testing).
  bool IsFakeWindow() const { return is_fake_window_; }

 private:
  Handle window_;
  const bool is_fake_window_;

  /// Creates a native window with the given handle. Handle ownership is assumed
  /// by this instance of the native window.
  explicit AndroidNativeWindow(Handle window);

  explicit AndroidNativeWindow(Handle window, bool is_fake_window);

  ~AndroidNativeWindow();

  FML_FRIEND_MAKE_REF_COUNTED(AndroidNativeWindow);
  FML_FRIEND_REF_COUNTED_THREAD_SAFE(AndroidNativeWindow);
  FML_DISALLOW_COPY_AND_ASSIGN(AndroidNativeWindow);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_ANDROID_SURFACE_ANDROID_NATIVE_WINDOW_H_
