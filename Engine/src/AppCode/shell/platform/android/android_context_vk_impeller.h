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

#ifndef APPCODE_SHELL_PLATFORM_ANDROID_ANDROID_CONTEXT_VK_IMPELLER_H_
#define APPCODE_SHELL_PLATFORM_ANDROID_ANDROID_CONTEXT_VK_IMPELLER_H_

#include "appcode/fml/macros.h"
#include "appcode/fml/native_library.h"
#include "appcode/shell/platform/android/context/android_context.h"

namespace appcode {

class AndroidContextVKImpeller : public AndroidContext {
 public:
  explicit AndroidContextVKImpeller(
      const AndroidContext::ContextSettings& settings);

  ~AndroidContextVKImpeller();

  // |AndroidContext|
  bool IsValid() const override;

  // |AndroidContext|
  AndroidRenderingAPI RenderingApi() const override;

 private:
  fml::RefPtr<fml::NativeLibrary> vulkan_dylib_;
  bool is_valid_ = false;

  FML_DISALLOW_COPY_AND_ASSIGN(AndroidContextVKImpeller);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_ANDROID_ANDROID_CONTEXT_VK_IMPELLER_H_
