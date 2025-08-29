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

#import "flutter/shell/platform/darwin/common/buffer_conversions.h"

#include "flutter/fml/macros.h"

static_assert(__has_feature(objc_arc), "ARC must be enabled.");

namespace flutter {
namespace {
class NSDataMapping : public fml::Mapping {
 public:
  explicit NSDataMapping(NSData* data) : data_(data) {}

  size_t GetSize() const override { return data_.length; }

  const uint8_t* GetMapping() const override { return static_cast<const uint8_t*>(data_.bytes); }

  bool IsDontNeedSafe() const override { return false; }

 private:
  NSData* data_;
  FML_DISALLOW_COPY_AND_ASSIGN(NSDataMapping);
};
}  // namespace

fml::MallocMapping CopyNSDataToMapping(NSData* data) {
  const uint8_t* bytes = static_cast<const uint8_t*>(data.bytes);
  return fml::MallocMapping::Copy(bytes, data.length);
}

NSData* ConvertMappingToNSData(fml::MallocMapping buffer) {
  size_t size = buffer.GetSize();
  return [NSData dataWithBytesNoCopy:buffer.Release() length:size];
}

std::unique_ptr<fml::Mapping> ConvertNSDataToMappingPtr(NSData* data) {
  return std::make_unique<NSDataMapping>(data);
}

NSData* CopyMappingPtrToNSData(std::unique_ptr<fml::Mapping> mapping) {
  return [NSData dataWithBytes:mapping->GetMapping() length:mapping->GetSize()];
}

}  // namespace flutter
