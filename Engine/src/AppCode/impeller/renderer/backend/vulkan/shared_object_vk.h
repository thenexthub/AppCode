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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_SHARED_OBJECT_VK_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_SHARED_OBJECT_VK_H_

#include <memory>

#include "impeller/renderer/backend/vulkan/vk.h"

namespace impeller {

class SharedObjectVK {
 public:
  virtual ~SharedObjectVK() = default;
};

template <class T>
class SharedObjectVKT : public SharedObjectVK {
 public:
  using Resource = T;
  using UniqueResource =
      vk::UniqueHandle<Resource, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE>;

  explicit SharedObjectVKT(UniqueResource res) : resource_(std::move(res)) {}

  // NOLINTNEXTLINE(google-explicit-constructor)
  operator Resource() const { return Get(); }

  const Resource& Get() const { return *resource_; }

  const UniqueResource& GetUniqueWrapper() const { return resource_; }

 private:
  UniqueResource resource_;

  SharedObjectVKT(const SharedObjectVKT&) = delete;

  SharedObjectVKT& operator=(const SharedObjectVKT&) = delete;
};

template <class T>
auto MakeSharedVK(
    vk::UniqueHandle<T, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE> handle) {
  if (!handle) {
    return std::shared_ptr<SharedObjectVKT<T>>{nullptr};
  }
  return std::make_shared<SharedObjectVKT<T>>(std::move(handle));
}

template <class T>
using SharedHandleVK = std::shared_ptr<SharedObjectVKT<T>>;

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_SHARED_OBJECT_VK_H_
