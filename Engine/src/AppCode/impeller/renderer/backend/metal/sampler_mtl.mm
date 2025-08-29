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

#include "impeller/renderer/backend/metal/sampler_mtl.h"

namespace impeller {

SamplerMTL::SamplerMTL(const SamplerDescriptor& desc, id<MTLSamplerState> state)
    : Sampler(desc), state_(state) {
  FML_DCHECK(state_);
}

SamplerMTL::~SamplerMTL() = default;

id<MTLSamplerState> SamplerMTL::GetMTLSamplerState() const {
  return state_;
}

}  // namespace impeller
