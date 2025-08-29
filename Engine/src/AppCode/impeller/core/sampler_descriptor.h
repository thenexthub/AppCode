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

#ifndef APPCODE_IMPELLER_CORE_SAMPLER_DESCRIPTOR_H_
#define APPCODE_IMPELLER_CORE_SAMPLER_DESCRIPTOR_H_

#include "impeller/core/formats.h"

namespace impeller {

class Context;

struct SamplerDescriptor final {
  MinMagFilter min_filter = MinMagFilter::kNearest;
  MinMagFilter mag_filter = MinMagFilter::kNearest;
  MipFilter mip_filter = MipFilter::kNearest;

  SamplerAddressMode width_address_mode = SamplerAddressMode::kClampToEdge;
  SamplerAddressMode height_address_mode = SamplerAddressMode::kClampToEdge;
  SamplerAddressMode depth_address_mode = SamplerAddressMode::kClampToEdge;

  std::string_view label = "NN Clamp Sampler";

  SamplerDescriptor();

  SamplerDescriptor(std::string_view label,
                    MinMagFilter min_filter,
                    MinMagFilter mag_filter,
                    MipFilter mip_filter);

  static uint64_t ToKey(const SamplerDescriptor& d) {
    static_assert(sizeof(MinMagFilter) == 1);
    static_assert(sizeof(MipFilter) == 1);
    static_assert(sizeof(SamplerAddressMode) == 1);

    return static_cast<uint64_t>(d.min_filter) << 0 |
           static_cast<uint64_t>(d.mag_filter) << 8 |
           static_cast<uint64_t>(d.mip_filter) << 16 |
           static_cast<uint64_t>(d.width_address_mode) << 24 |
           static_cast<uint64_t>(d.height_address_mode) << 32 |
           static_cast<uint64_t>(d.depth_address_mode) << 40;
  }
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_CORE_SAMPLER_DESCRIPTOR_H_
