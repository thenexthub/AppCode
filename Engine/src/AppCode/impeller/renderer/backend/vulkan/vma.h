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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_VMA_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_VMA_H_

#include "appcode/appcode_vma/appcode_vma.h"
#include "appcode/fml/trace_event.h"
#include "appcode/fml/unique_object.h"
#include "impeller/renderer/backend/vulkan/vk.h"

namespace impeller {

// -----------------------------------------------------------------------------
// Unique handles to VMA allocators.
// -----------------------------------------------------------------------------
struct AllocatorVMATraits {
  static VmaAllocator InvalidValue() { return {}; }

  static bool IsValid(const VmaAllocator& value) {
    return value != InvalidValue();
  }

  static void Free(VmaAllocator allocator) {
    TRACE_EVENT0("impeller", "DestroyAllocator");
    ::vmaDestroyAllocator(allocator);
  }
};

using UniqueAllocatorVMA = fml::UniqueObject<VmaAllocator, AllocatorVMATraits>;

// -----------------------------------------------------------------------------
// Unique handles to VMA pools.
// -----------------------------------------------------------------------------

struct PoolVMA {
  VmaAllocator allocator = {};
  VmaPool pool = {};

  constexpr bool operator==(const PoolVMA& other) const {
    return allocator == other.allocator && pool == other.pool;
  }

  constexpr bool operator!=(const PoolVMA& other) const {
    return !(*this == other);
  }
};

struct PoolVMATraits {
  static PoolVMA InvalidValue() { return {}; }

  static bool IsValid(const PoolVMA& value) {
    return value.allocator != VmaAllocator{};
  }

  static void Free(const PoolVMA& pool) {
    TRACE_EVENT0("impeller", "DestroyPool");
    ::vmaDestroyPool(pool.allocator, pool.pool);
  }
};

using UniquePoolVMA = fml::UniqueObject<PoolVMA, PoolVMATraits>;

// -----------------------------------------------------------------------------
// Unique handles to VMA buffers.
// -----------------------------------------------------------------------------

struct BufferVMA {
  VmaAllocator allocator = {};
  VmaAllocation allocation = {};
  vk::Buffer buffer = {};

  constexpr bool operator==(const BufferVMA& other) const {
    return allocator == other.allocator && allocation == other.allocation &&
           buffer == other.buffer;
  }

  constexpr bool operator!=(const BufferVMA& other) const {
    return !(*this == other);
  }
};

struct BufferVMATraits {
  static BufferVMA InvalidValue() { return {}; }

  static bool IsValid(const BufferVMA& value) {
    return value.allocator != VmaAllocator{};
  }

  static void Free(const BufferVMA& buffer) {
    TRACE_EVENT0("impeller", "DestroyBuffer");
    ::vmaDestroyBuffer(buffer.allocator, static_cast<VkBuffer>(buffer.buffer),
                       buffer.allocation);
  }
};

using UniqueBufferVMA = fml::UniqueObject<BufferVMA, BufferVMATraits>;

// -----------------------------------------------------------------------------
// Unique handles to VMA images.
// -----------------------------------------------------------------------------

struct ImageVMA {
  VmaAllocator allocator = {};
  VmaAllocation allocation = {};
  vk::Image image = {};

  constexpr bool operator==(const ImageVMA& other) const {
    return allocator == other.allocator && allocation == other.allocation &&
           image == other.image;
  }

  constexpr bool operator!=(const ImageVMA& other) const {
    return !(*this == other);
  }
};

struct ImageVMATraits {
  static ImageVMA InvalidValue() { return {}; }

  static bool IsValid(const ImageVMA& value) {
    return value.allocator != VmaAllocator{};
  }

  static void Free(const ImageVMA& image) {
    TRACE_EVENT0("impeller", "DestroyImage");
    ::vmaDestroyImage(image.allocator, static_cast<VkImage>(image.image),
                      image.allocation);
  }
};

using UniqueImageVMA = fml::UniqueObject<ImageVMA, ImageVMATraits>;

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_VMA_H_
