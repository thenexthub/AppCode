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

#include "impeller/core/texture_descriptor.h"

#include <sstream>

namespace impeller {

std::string TextureDescriptorToString(const TextureDescriptor& desc) {
  std::stringstream stream;
  stream << "StorageMode=" << StorageModeToString(desc.storage_mode) << ",";
  stream << "Type=" << TextureTypeToString(desc.type) << ",";
  stream << "Format=" << PixelFormatToString(desc.format) << ",";
  stream << "Size=" << desc.size << ",";
  stream << "MipCount=" << desc.mip_count << ",";
  stream << "SampleCount=" << static_cast<size_t>(desc.sample_count) << ",";
  stream << "Compression=" << CompressionTypeToString(desc.compression_type);
  return stream.str();
}

}  // namespace impeller
