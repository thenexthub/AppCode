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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_METAL_ALLOCATOR_MTL_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_METAL_ALLOCATOR_MTL_H_

#include <Metal/Metal.h>
#include <atomic>

#include "impeller/base/thread.h"
#include "impeller/core/allocator.h"

namespace impeller {

class DebugAllocatorStats {
 public:
  DebugAllocatorStats() {}

  ~DebugAllocatorStats() {}

  /// Increment the tracked allocation size in bytes.
  void Increment(size_t size);

  /// Decrement the tracked allocation size in bytes.
  void Decrement(size_t size);

  /// Get the current tracked allocation size.
  Bytes GetAllocationSize();

 private:
  std::atomic<size_t> size_ = 0;
};

ISize DeviceMaxTextureSizeSupported(id<MTLDevice> device);

class AllocatorMTL final : public Allocator {
 public:
  AllocatorMTL();

  // |Allocator|
  ~AllocatorMTL() override;

  // |Allocator|
  Bytes DebugGetHeapUsage() const override;

  // visible for testing.
  void DebugSetSupportsUMA(bool value);

  AllocatorMTL(id<MTLDevice> device, std::string label);

 private:
  friend class ContextMTL;

  id<MTLDevice> device_;
  std::string allocator_label_;
  bool supports_memoryless_targets_ = false;
  bool supports_uma_ = false;
  bool is_valid_ = false;

#ifdef IMPELLER_DEBUG
  std::shared_ptr<DebugAllocatorStats> debug_allocater_ =
      std::make_shared<DebugAllocatorStats>();
#endif  // IMPELLER_DEBUG

  ISize max_texture_supported_;

  // |Allocator|
  bool IsValid() const;

  // |Allocator|
  std::shared_ptr<DeviceBuffer> OnCreateBuffer(
      const DeviceBufferDescriptor& desc) override;

  // |Allocator|
  std::shared_ptr<Texture> OnCreateTexture(
      const TextureDescriptor& desc) override;

  // |Allocator|
  uint16_t MinimumBytesPerRow(PixelFormat format) const override;

  // |Allocator|
  ISize GetMaxTextureSizeSupported() const override;

  // |Allocator|
  void DebugTraceMemoryStatistics() const override;

  AllocatorMTL(const AllocatorMTL&) = delete;

  AllocatorMTL& operator=(const AllocatorMTL&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_METAL_ALLOCATOR_MTL_H_
