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

#ifndef APPCODE_SHELL_PLATFORM_ANDROID_ANDROID_CONTEXT_DYNAMIC_IMPELLER_H_
#define APPCODE_SHELL_PLATFORM_ANDROID_ANDROID_CONTEXT_DYNAMIC_IMPELLER_H_

#include <mutex>

#include "appcode/fml/macros.h"
#include "appcode/fml/native_library.h"
#include "appcode/shell/platform/android/android_context_gl_impeller.h"
#include "appcode/shell/platform/android/android_context_vk_impeller.h"
#include "appcode/shell/platform/android/context/android_context.h"

namespace appcode {

/// @brief An Impeller Android context that dynamically creates either an
/// [AndroidContextGLImpeller] or an [AndroidContextVKImpeller].
///
/// The construction of these objects is deferred until [GetImpellerContext] is
/// invoked. Up to this point, the reported backend will be kImpellerAutoselect.
class AndroidContextDynamicImpeller : public AndroidContext {
 public:
  explicit AndroidContextDynamicImpeller(
      const AndroidContext::ContextSettings& settings);

  ~AndroidContextDynamicImpeller();

  // |AndroidContext|
  bool IsValid() const override { return true; }

  // |AndroidContext|
  AndroidRenderingAPI RenderingApi() const override;

  /// @brief Retrieve the GL Context if it was created, or nullptr.
  std::shared_ptr<AndroidContextGLImpeller> GetGLContext() const;

  /// @brief Retrieve the VK context if it was created, or nullptr.
  std::shared_ptr<AndroidContextVKImpeller> GetVKContext() const;

  // |AndroidContext|
  void SetupImpellerContext() override;

  std::shared_ptr<impeller::Context> GetImpellerContext() const override;

 private:
  const AndroidContext::ContextSettings settings_;
  std::shared_ptr<AndroidContextGLImpeller> gl_context_;
  std::shared_ptr<AndroidContextVKImpeller> vk_context_;

  FML_DISALLOW_COPY_AND_ASSIGN(AndroidContextDynamicImpeller);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_ANDROID_ANDROID_CONTEXT_DYNAMIC_IMPELLER_H_
