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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_GLES_DEVICE_BUFFER_GLES_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_GLES_DEVICE_BUFFER_GLES_H_

#include <cstdint>
#include <memory>

#include "impeller/base/allocation.h"
#include "impeller/base/backend_cast.h"
#include "impeller/core/device_buffer.h"
#include "impeller/renderer/backend/gles/reactor_gles.h"

namespace impeller {

class DeviceBufferGLES final
    : public DeviceBuffer,
      public BackendCast<DeviceBufferGLES, DeviceBuffer> {
 public:
  DeviceBufferGLES(DeviceBufferDescriptor desc,
                   std::shared_ptr<ReactorGLES> reactor,
                   std::shared_ptr<Allocation> backing_store);

  // |DeviceBuffer|
  ~DeviceBufferGLES() override;

  const uint8_t* GetBufferData() const;

  void UpdateBufferData(
      const std::function<void(uint8_t*, size_t length)>& update_buffer_data);

  enum class BindingType {
    kArrayBuffer,
    kElementArrayBuffer,
    kUniformBuffer,
  };

  [[nodiscard]] bool BindAndUploadDataIfNecessary(BindingType type) const;

  void Flush(std::optional<Range> range = std::nullopt) const override;

  std::optional<GLuint> GetHandle() const;

 private:
  std::shared_ptr<ReactorGLES> reactor_;
  std::optional<std::string> label_;
  // Mutable for lazy evaluation.
  mutable std::optional<HandleGLES> handle_;
  mutable std::shared_ptr<Allocation> backing_store_;
  mutable std::optional<Range> dirty_range_ = std::nullopt;
  mutable bool initialized_ = false;

  // |DeviceBuffer|
  uint8_t* OnGetContents() const override;

  // |DeviceBuffer|
  bool OnCopyHostBuffer(const uint8_t* source,
                        Range source_range,
                        size_t offset) override;

  // |DeviceBuffer|
  bool SetLabel(std::string_view label) override;

  // |DeviceBuffer|
  bool SetLabel(std::string_view label, Range range) override;

  DeviceBufferGLES(const DeviceBufferGLES&) = delete;

  DeviceBufferGLES& operator=(const DeviceBufferGLES&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_GLES_DEVICE_BUFFER_GLES_H_
