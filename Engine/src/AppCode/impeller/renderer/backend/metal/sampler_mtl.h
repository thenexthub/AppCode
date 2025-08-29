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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_METAL_SAMPLER_MTL_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_METAL_SAMPLER_MTL_H_

#include <Metal/Metal.h>

#include "impeller/base/backend_cast.h"
#include "impeller/core/sampler.h"

namespace impeller {

class SamplerLibraryMTL;

class SamplerMTL final : public Sampler,
                         public BackendCast<SamplerMTL, Sampler> {
 public:
  SamplerMTL();

  // |Sampler|
  ~SamplerMTL() override;

  id<MTLSamplerState> GetMTLSamplerState() const;

 private:
  friend SamplerLibraryMTL;

  id<MTLSamplerState> state_ = nullptr;

  SamplerMTL(const SamplerDescriptor& desc, id<MTLSamplerState> state);

  SamplerMTL(const SamplerMTL&) = delete;

  SamplerMTL& operator=(const SamplerMTL&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_METAL_SAMPLER_MTL_H_
