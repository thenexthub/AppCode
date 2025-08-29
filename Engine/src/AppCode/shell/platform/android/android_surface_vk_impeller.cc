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

#include "appcode/shell/platform/android/android_surface_vk_impeller.h"

#include <memory>
#include <utility>

#include "appcode/fml/concurrent_message_loop.h"
#include "appcode/fml/logging.h"
#include "appcode/fml/memory/ref_ptr.h"
#include "appcode/impeller/renderer/backend/vulkan/context_vk.h"
#include "appcode/impeller/renderer/backend/vulkan/swapchain/swapchain_vk.h"
#include "appcode/shell/gpu/gpu_surface_vulkan_impeller.h"
#include "appcode/vulkan/vulkan_native_surface_android.h"
#include "impeller/renderer/backend/vulkan/swapchain/ahb/ahb_swapchain_vk.h"
#include "impeller/toolkit/android/surface_transaction.h"

namespace appcode {

AndroidSurfaceVKImpeller::AndroidSurfaceVKImpeller(
    const std::shared_ptr<AndroidContextVKImpeller>& android_context) {
  is_valid_ = android_context->IsValid();

  auto& context_vk =
      impeller::ContextVK::Cast(*android_context->GetImpellerContext());
  surface_context_vk_ = context_vk.CreateSurfaceContext();
}

AndroidSurfaceVKImpeller::~AndroidSurfaceVKImpeller() = default;

bool AndroidSurfaceVKImpeller::IsValid() const {
  return is_valid_;
}

void AndroidSurfaceVKImpeller::TeardownOnScreenContext() {
  surface_context_vk_->TeardownSwapchain();
}

std::unique_ptr<Surface> AndroidSurfaceVKImpeller::CreateGPUSurface(
    GrDirectContext* gr_context) {
  if (!IsValid()) {
    return nullptr;
  }

  if (!native_window_ || !native_window_->IsValid()) {
    return nullptr;
  }

  std::unique_ptr<GPUSurfaceVulkanImpeller> gpu_surface =
      std::make_unique<GPUSurfaceVulkanImpeller>(nullptr, surface_context_vk_);

  if (!gpu_surface->IsValid()) {
    return nullptr;
  }

  return gpu_surface;
}

bool AndroidSurfaceVKImpeller::OnScreenSurfaceResize(const SkISize& size) {
  surface_context_vk_->UpdateSurfaceSize(
      impeller::ISize{size.width(), size.height()});
  return true;
}

bool AndroidSurfaceVKImpeller::ResourceContextMakeCurrent() {
  return true;
}

bool AndroidSurfaceVKImpeller::ResourceContextClearCurrent() {
  return true;
}

bool AndroidSurfaceVKImpeller::SetNativeWindow(
    fml::RefPtr<AndroidNativeWindow> window,
    const std::shared_ptr<PlatformViewAndroidJNI>& jni_facade) {
  if (window && (native_window_ == window)) {
    return OnScreenSurfaceResize(window->GetSize());
  }

  native_window_ = nullptr;

  if (!window || !window->IsValid()) {
    return false;
  }

  impeller::CreateTransactionCB cb = [jni_facade = jni_facade]() {
    FML_CHECK(jni_facade) << "JNI was nullptr";
    ASurfaceTransaction* tx = jni_facade->createTransaction();
    if (tx == nullptr) {
      return impeller::android::SurfaceTransaction();
    }
    return impeller::android::SurfaceTransaction(tx);
  };

  auto swapchain = impeller::SwapchainVK::Create(
      std::reinterpret_pointer_cast<impeller::Context>(
          surface_context_vk_->GetParent()),
      window->handle(), cb);

  if (surface_context_vk_->SetSwapchain(std::move(swapchain))) {
    native_window_ = std::move(window);
    return true;
  }

  return false;
}

std::shared_ptr<impeller::Context>
AndroidSurfaceVKImpeller::GetImpellerContext() {
  return surface_context_vk_;
}

}  // namespace appcode
