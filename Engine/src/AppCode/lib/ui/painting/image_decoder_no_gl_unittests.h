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

#ifndef APPCODE_LIB_UI_PAINTING_IMAGE_DECODER_NO_GL_UNITTESTS_H_
#define APPCODE_LIB_UI_PAINTING_IMAGE_DECODER_NO_GL_UNITTESTS_H_

#include <stdint.h>

#include "appcode/impeller/core/allocator.h"
#include "appcode/impeller/core/device_buffer.h"
#include "appcode/impeller/core/formats.h"
#include "appcode/impeller/geometry/size.h"
#include "appcode/lib/ui/painting/image_decoder.h"
#include "appcode/lib/ui/painting/image_decoder_impeller.h"
#include "appcode/testing/testing.h"

namespace impeller {

class TestImpellerTexture : public Texture {
 public:
  explicit TestImpellerTexture(TextureDescriptor desc) : Texture(desc) {}

  void SetLabel(std::string_view label) override {}
  void SetLabel(std::string_view label, std::string_view trailing) override {}
  bool IsValid() const override { return true; }
  ISize GetSize() const { return GetTextureDescriptor().size; }

  bool OnSetContents(const uint8_t* contents, size_t length, size_t slice) {
    return true;
  }
  bool OnSetContents(std::shared_ptr<const fml::Mapping> mapping,
                     size_t slice) {
    return true;
  }
};

class TestImpellerDeviceBuffer : public DeviceBuffer {
 public:
  explicit TestImpellerDeviceBuffer(DeviceBufferDescriptor desc)
      : DeviceBuffer(desc) {
    bytes_ = static_cast<uint8_t*>(malloc(desc.size));
  }

  ~TestImpellerDeviceBuffer() { free(bytes_); }

 private:
  bool SetLabel(std::string_view label) override { return true; }

  bool SetLabel(std::string_view label, Range range) override { return true; }

  uint8_t* OnGetContents() const override { return bytes_; }

  bool OnCopyHostBuffer(const uint8_t* source,
                        Range source_range,
                        size_t offset) override {
    for (auto i = source_range.offset; i < source_range.length; i++, offset++) {
      bytes_[offset] = source[i];
    }
    return true;
  }

  uint8_t* bytes_;
};

class TestImpellerAllocator : public impeller::Allocator {
 public:
  TestImpellerAllocator() {}

  ~TestImpellerAllocator() = default;

 private:
  uint16_t MinimumBytesPerRow(PixelFormat format) const override { return 0; }

  ISize GetMaxTextureSizeSupported() const override {
    return ISize{2048, 2048};
  }

  std::shared_ptr<DeviceBuffer> OnCreateBuffer(
      const DeviceBufferDescriptor& desc) override {
    return std::make_shared<TestImpellerDeviceBuffer>(desc);
  }

  std::shared_ptr<Texture> OnCreateTexture(
      const TextureDescriptor& desc) override {
    return std::make_shared<TestImpellerTexture>(desc);
  }
};

}  // namespace impeller

namespace appcode {
namespace testing {

float HalfToFloat(uint16_t half);
float DecodeBGR10(uint32_t x);

}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_LIB_UI_PAINTING_IMAGE_DECODER_NO_GL_UNITTESTS_H_
