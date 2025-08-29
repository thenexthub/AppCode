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

#ifndef APPCODE_VULKAN_PROCS_VULKAN_HANDLE_H_
#define APPCODE_VULKAN_PROCS_VULKAN_HANDLE_H_

#include <functional>

#include "appcode/fml/logging.h"
#include "appcode/fml/macros.h"
#include "appcode/vulkan/procs/vulkan_interface.h"

namespace vulkan {

template <class T>
class VulkanHandle {
 public:
  using Handle = T;
  using Disposer = std::function<void(Handle)>;

  VulkanHandle() : handle_(VK_NULL_HANDLE) {}

  explicit VulkanHandle(Handle handle, const Disposer& disposer = nullptr)
      : handle_(handle), disposer_(disposer) {}

  VulkanHandle(VulkanHandle&& other)
      : handle_(other.handle_), disposer_(std::move(other.disposer_)) {
    other.handle_ = VK_NULL_HANDLE;
    other.disposer_ = nullptr;
  }

  ~VulkanHandle() { DisposeIfNecessary(); }

  VulkanHandle& operator=(VulkanHandle&& other) {
    if (handle_ != other.handle_) {
      DisposeIfNecessary();
    }

    handle_ = other.handle_;
    disposer_ = other.disposer_;

    other.handle_ = VK_NULL_HANDLE;
    other.disposer_ = nullptr;

    return *this;
  }

  explicit operator bool() const { return handle_ != VK_NULL_HANDLE; }

  // NOLINTNEXTLINE(google-explicit-constructor)
  operator Handle() const { return handle_; }

  /// Relinquish responsibility of collecting the underlying handle when this
  /// object is collected. It is the responsibility of the caller to ensure that
  /// the lifetime of the handle extends past the lifetime of this object.
  void ReleaseOwnership() { disposer_ = nullptr; }

  void Reset() { DisposeIfNecessary(); }

 private:
  Handle handle_;
  Disposer disposer_;

  void DisposeIfNecessary() {
    if (handle_ == VK_NULL_HANDLE) {
      return;
    }
    if (disposer_) {
      disposer_(handle_);
    }
    handle_ = VK_NULL_HANDLE;
    disposer_ = nullptr;
  }

  FML_DISALLOW_COPY_AND_ASSIGN(VulkanHandle);
};

}  // namespace vulkan

#endif  // APPCODE_VULKAN_PROCS_VULKAN_HANDLE_H_
