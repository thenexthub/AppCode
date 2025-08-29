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

#ifndef APPCODE_SHELL_PLATFORM_ANDROID_PLATFORM_VIEW_ANDROID_DELEGATE_PLATFORM_VIEW_ANDROID_DELEGATE_H_
#define APPCODE_SHELL_PLATFORM_ANDROID_PLATFORM_VIEW_ANDROID_DELEGATE_PLATFORM_VIEW_ANDROID_DELEGATE_H_

#include <memory>
#include <string>
#include <vector>

#include "appcode/shell/common/platform_view.h"
#include "appcode/shell/platform/android/jni/platform_view_android_jni.h"

namespace appcode {

class PlatformViewAndroidDelegate {
 public:
  static constexpr size_t kBytesPerNode = 50 * sizeof(int32_t);
  static constexpr size_t kBytesPerChild = sizeof(int32_t);
  static constexpr size_t kBytesPerCustomAction = sizeof(int32_t);
  static constexpr size_t kBytesPerAction = 4 * sizeof(int32_t);
  static constexpr size_t kBytesPerStringAttribute = 4 * sizeof(int32_t);
  explicit PlatformViewAndroidDelegate(
      std::shared_ptr<PlatformViewAndroidJNI> jni_facade);
  void UpdateSemantics(
      const appcode::SemanticsNodeUpdates& update,
      const appcode::CustomAccessibilityActionUpdates& actions);

 private:
  const std::shared_ptr<PlatformViewAndroidJNI> jni_facade_;
};
}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_ANDROID_PLATFORM_VIEW_ANDROID_DELEGATE_PLATFORM_VIEW_ANDROID_DELEGATE_H_
