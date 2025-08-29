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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_METAL_SAMPLER_LIBRARY_MTL_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_METAL_SAMPLER_LIBRARY_MTL_H_

#include <Metal/Metal.h>

#include <memory>

#include "impeller/base/backend_cast.h"
#include "impeller/base/comparable.h"
#include "impeller/core/sampler_descriptor.h"
#include "impeller/renderer/sampler_library.h"

namespace impeller {

class SamplerLibraryMTL final
    : public SamplerLibrary,
      public BackendCast<SamplerLibraryMTL, SamplerLibrary> {
 public:
  // |SamplerLibrary|
  ~SamplerLibraryMTL() override;

 private:
  friend class ContextMTL;

  id<MTLDevice> device_ = nullptr;
  std::vector<std::pair<uint64_t, std::shared_ptr<const Sampler>>> samplers_;

  explicit SamplerLibraryMTL(id<MTLDevice> device);

  // |SamplerLibrary|
  raw_ptr<const Sampler> GetSampler(
      const SamplerDescriptor& descriptor) override;

  SamplerLibraryMTL(const SamplerLibraryMTL&) = delete;

  SamplerLibraryMTL& operator=(const SamplerLibraryMTL&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_METAL_SAMPLER_LIBRARY_MTL_H_
