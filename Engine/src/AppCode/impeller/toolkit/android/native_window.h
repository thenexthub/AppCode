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

#ifndef APPCODE_IMPELLER_TOOLKIT_ANDROID_NATIVE_WINDOW_H_
#define APPCODE_IMPELLER_TOOLKIT_ANDROID_NATIVE_WINDOW_H_

#include "appcode/fml/unique_object.h"
#include "impeller/geometry/size.h"
#include "impeller/toolkit/android/proc_table.h"

namespace impeller::android {

//------------------------------------------------------------------------------
/// @brief      A wrapper for ANativeWindow
///             https://developer.android.com/ndk/reference/group/a-native-window
///
///             This wrapper is only available on Android.
///
class NativeWindow {
 public:
  explicit NativeWindow(ANativeWindow* window);

  ~NativeWindow();

  NativeWindow(const NativeWindow&) = delete;

  NativeWindow& operator=(const NativeWindow&) = delete;

  bool IsValid() const;

  //----------------------------------------------------------------------------
  /// @return     The current size of the native window.
  ///
  ISize GetSize() const;

  ANativeWindow* GetHandle() const;

 private:
  struct UniqueANativeWindowTraits {
    static ANativeWindow* InvalidValue() { return nullptr; }

    static bool IsValid(ANativeWindow* value) {
      return value != InvalidValue();
    }

    static void Free(ANativeWindow* value) {
      GetProcTable().ANativeWindow_release(value);
    }
  };

  fml::UniqueObject<ANativeWindow*, UniqueANativeWindowTraits> window_;
};

}  // namespace impeller::android

#endif  // APPCODE_IMPELLER_TOOLKIT_ANDROID_NATIVE_WINDOW_H_
