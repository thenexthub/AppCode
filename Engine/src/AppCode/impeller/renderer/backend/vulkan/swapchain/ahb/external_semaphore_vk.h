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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_SWAPCHAIN_AHB_EXTERNAL_SEMAPHORE_VK_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_SWAPCHAIN_AHB_EXTERNAL_SEMAPHORE_VK_H_

#include "appcode/fml/unique_fd.h"
#include "impeller/renderer/backend/vulkan/shared_object_vk.h"
#include "impeller/renderer/backend/vulkan/vk.h"
#include "impeller/renderer/context.h"
#include "vulkan/vulkan_handles.hpp"

namespace impeller {

//------------------------------------------------------------------------------
/// @brief      A Vulkan semaphore that can be exported as a platform specific
///             file descriptor.
///
///             The semaphore are exported as sync file descriptors.
///
/// @warning    Only semaphore that have been signaled or have a single
///             operation pending can be exported. Make sure to submit a fence
///             signalling operation to a queue before attempted to obtain a
///             file descriptor for the fence.
///
class ExternalSemaphoreVK {
 public:
  //----------------------------------------------------------------------------
  /// @brief      Create a new un-signaled semaphore that can be exported as a
  ///             sync file descriptor.
  ///
  /// @param[in]  context  The device context.
  ///
  explicit ExternalSemaphoreVK(const std::shared_ptr<Context>& context);

  ~ExternalSemaphoreVK();

  ExternalSemaphoreVK(const ExternalSemaphoreVK&) = delete;

  ExternalSemaphoreVK& operator=(const ExternalSemaphoreVK&) = delete;

  //----------------------------------------------------------------------------
  /// @brief      If a valid fence could be created.
  ///
  /// @return     True if valid, False otherwise.
  ///
  bool IsValid() const;

  //----------------------------------------------------------------------------
  /// @brief      Create a new sync file descriptor for the underlying
  ///             semaphore.
  ///
  ///             The semaphore must already be signaled or have a signal
  ///             operation pending in a queue. There are no checks for this in
  ///             the implementation and only Vulkan validation will catch such
  ///             a misuse and undefined behavior.
  ///
  /// @warning    Implementations are also allowed to return invalid file
  ///             descriptors in case a semaphore has already been signaled. So
  ///             it is not necessary an error to obtain an invalid descriptor
  ///             from this call. For APIs that are meant to consume such
  ///             descriptors, pass -1 as the file handle.
  ///
  ///             Since this call can return an invalid FD even in case of
  ///             success, make sure to make the `IsValid` check before
  ///             attempting to export a FD.
  ///
  /// @return     A (potentially invalid even in case of success) file
  ///             descriptor.
  ///
  fml::UniqueFD CreateFD() const;

  const vk::Semaphore& GetHandle() const;

  const SharedHandleVK<vk::Semaphore>& GetSharedHandle() const;

 private:
  SharedHandleVK<vk::Semaphore> semaphore_;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_SWAPCHAIN_AHB_EXTERNAL_SEMAPHORE_VK_H_
