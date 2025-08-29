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

#include "impeller/renderer/backend/vulkan/swapchain/ahb/external_semaphore_vk.h"

#include "impeller/base/validation.h"
#include "impeller/renderer/backend/vulkan/context_vk.h"
#include "vulkan/vulkan_handles.hpp"

namespace impeller {

ExternalSemaphoreVK::ExternalSemaphoreVK(
    const std::shared_ptr<Context>& context) {
  if (!context) {
    return;
  }
  vk::StructureChain<vk::SemaphoreCreateInfo, vk::ExportSemaphoreCreateInfoKHR>
      info;

  info.get<vk::ExportSemaphoreCreateInfoKHR>().handleTypes =
      vk::ExternalSemaphoreHandleTypeFlagBits::eSyncFd;

  const auto& context_vk = ContextVK::Cast(*context);
  auto [result, semaphore] =
      context_vk.GetDevice().createSemaphoreUnique(info.get());
  if (result != vk::Result::eSuccess) {
    VALIDATION_LOG << "Could not create external fence: "
                   << vk::to_string(result);
    return;
  }

  context_vk.SetDebugName(semaphore.get(), "ExternalSemaphoreSyncFD");

  semaphore_ = MakeSharedVK(std::move(semaphore));
}

ExternalSemaphoreVK::~ExternalSemaphoreVK() = default;

bool ExternalSemaphoreVK::IsValid() const {
  return !!semaphore_;
}

fml::UniqueFD ExternalSemaphoreVK::CreateFD() const {
  if (!IsValid()) {
    return {};
  }
  vk::SemaphoreGetFdInfoKHR info;
  info.semaphore = semaphore_->Get();
  info.handleType = vk::ExternalSemaphoreHandleTypeFlagBits::eSyncFd;
  auto [result, fd] =
      semaphore_->GetUniqueWrapper().getOwner().getSemaphoreFdKHR(info);
  if (result != vk::Result::eSuccess) {
    VALIDATION_LOG << "Could not export external fence FD: "
                   << vk::to_string(result);
    return {};
  }
  return fml::UniqueFD{fd};
}

const vk::Semaphore& ExternalSemaphoreVK::GetHandle() const {
  return semaphore_->Get();
}

const SharedHandleVK<vk::Semaphore>& ExternalSemaphoreVK::GetSharedHandle()
    const {
  return semaphore_;
}

}  // namespace impeller
