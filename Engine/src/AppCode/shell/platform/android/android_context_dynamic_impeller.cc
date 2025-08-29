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

#include "appcode/shell/platform/android/android_context_dynamic_impeller.h"

#include <android/api-level.h>
#include <sys/system_properties.h>
#include <memory>

#include "appcode/impeller/base/validation.h"
#include "shell/platform/android/android_rendering_selector.h"

namespace appcode {

namespace {

static const constexpr char* kAndroidHuawei = "android-huawei";

/// These are SoCs that crash when using AHB imports.
static constexpr const char* kBLC[] = {
    // Most Exynos Series SoC
    "exynos7870",  //
    "exynos7880",  //
    "exynos7872",  //
    "exynos7884",  //
    "exynos7885",  //
    "exynos8890",  //
    "exynos8895",  //
    "exynos7904",  //
    "exynos9609",  //
    "exynos9610",  //
    "exynos9611",  //
    "exynos9810"   //
};

static bool IsDeviceEmulator(std::string_view product_model) {
  return std::string(product_model).find("gphone") != std::string::npos;
}

static bool IsKnownBadSOC(std::string_view hardware) {
  // TODO(jonahwilliams): if the list gets too long (> 16), convert
  // to a hash map first.
  for (const auto& board : kBLC) {
    if (strcmp(board, hardware.data()) == 0) {
      return true;
    }
  }
  return false;
}

static std::shared_ptr<AndroidContextVKImpeller>
GetActualRenderingAPIForImpeller(
    int api_level,
    const AndroidContext::ContextSettings& settings) {
  constexpr int kMinimumAndroidApiLevelForMediaTekVulkan = 31;

  // have requisite features to support platform views.
  //
  // Even if this check returns true, Impeller may determine it cannot use
  // Vulkan for some other reason, such as a missing required extension or
  // feature. In these cases it will use OpenGLES.
  char product_model[PROP_VALUE_MAX];
  __system_property_get("ro.product.model", product_model);
  if (IsDeviceEmulator(product_model)) {
    // Avoid using Vulkan on known emulators.
    return nullptr;
  }

  __system_property_get("ro.com.google.clientidbase", product_model);
  if (strcmp(product_model, kAndroidHuawei) == 0) {
    // Avoid using Vulkan on Huawei as AHB imports do not
    // consistently work.
    return nullptr;
  }

  if (api_level < kMinimumAndroidApiLevelForMediaTekVulkan &&
      __system_property_find("ro.vendor.mediatek.platform") != nullptr) {
    // Probably MediaTek. Avoid Vulkan if older than 34 to work around
    // crashes when importing AHB.
    return nullptr;
  }

  __system_property_get("ro.product.board", product_model);
  if (IsKnownBadSOC(product_model)) {
    // Avoid using Vulkan on known bad SoCs.
    return nullptr;
  }

  // Determine if Vulkan is supported by creating a Vulkan context and
  // checking if it is valid.
  impeller::ScopedValidationDisable disable_validation;
  auto vulkan_backend = std::make_shared<AndroidContextVKImpeller>(
      AndroidContext::ContextSettings{
#if appcode_RUNTIME_MODE == appcode_RUNTIME_MODE_DEBUG
          .enable_validation = settings.enable_validation,
#else
          .enable_validation = false,
#endif  // appcode_RUNTIME_MODE == appcode_RUNTIME_MODE_DEBUG
          .enable_gpu_tracing = settings.enable_gpu_tracing,
          .enable_surface_control = settings.enable_surface_control,
          .impeller_flags =
              {
                  .lazy_shader_mode = settings.impeller_flags.lazy_shader_mode,
                  .antialiased_lines =
                      settings.impeller_flags.antialiased_lines,
              },
      });
  if (!vulkan_backend->IsValid()) {
    return nullptr;
  }
  return vulkan_backend;
}
}  // namespace

AndroidContextDynamicImpeller::AndroidContextDynamicImpeller(
    const AndroidContext::ContextSettings& settings)
    : AndroidContext(AndroidRenderingAPI::kImpellerVulkan),
      settings_(settings) {}

AndroidContextDynamicImpeller::~AndroidContextDynamicImpeller() = default;

AndroidRenderingAPI AndroidContextDynamicImpeller::RenderingApi() const {
  if (vk_context_) {
    return AndroidRenderingAPI::kImpellerVulkan;
  }
  if (gl_context_) {
    return AndroidRenderingAPI::kImpellerOpenGLES;
  }
  return AndroidRenderingAPI::kImpellerAutoselect;
}

std::shared_ptr<impeller::Context>
AndroidContextDynamicImpeller::GetImpellerContext() const {
  if (vk_context_) {
    return vk_context_->GetImpellerContext();
  }
  if (gl_context_) {
    return gl_context_->GetImpellerContext();
  }
  return nullptr;
}

std::shared_ptr<AndroidContextGLImpeller>
AndroidContextDynamicImpeller::GetGLContext() const {
  return gl_context_;
}

std::shared_ptr<AndroidContextVKImpeller>
AndroidContextDynamicImpeller::GetVKContext() const {
  return vk_context_;
}

void AndroidContextDynamicImpeller::SetupImpellerContext() {
  if (vk_context_ || gl_context_) {
    return;
  }
  vk_context_ = GetActualRenderingAPIForImpeller(android_get_device_api_level(),
                                                 settings_);
  if (!vk_context_) {
    gl_context_ = std::make_shared<AndroidContextGLImpeller>(
        std::make_unique<impeller::egl::Display>(),
        settings_.enable_gpu_tracing);
  }
}

}  // namespace appcode
