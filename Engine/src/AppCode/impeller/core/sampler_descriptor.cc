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

#include "impeller/core/sampler_descriptor.h"

namespace impeller {

SamplerDescriptor::SamplerDescriptor() = default;

SamplerDescriptor::SamplerDescriptor(std::string_view label,
                                     MinMagFilter min_filter,
                                     MinMagFilter mag_filter,
                                     MipFilter mip_filter)
    : min_filter(min_filter),
      mag_filter(mag_filter),
      mip_filter(mip_filter),
      label(label) {}

}  // namespace impeller
