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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_GLES_SAMPLER_LIBRARY_GLES_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_GLES_SAMPLER_LIBRARY_GLES_H_

#include "impeller/core/sampler.h"
#include "impeller/core/sampler_descriptor.h"
#include "impeller/renderer/backend/gles/sampler_gles.h"
#include "impeller/renderer/sampler_library.h"

namespace impeller {

class SamplerLibraryGLES final : public SamplerLibrary {
 public:
  explicit SamplerLibraryGLES(bool supports_decal_sampler_address_mode);
  // |SamplerLibrary|
  ~SamplerLibraryGLES() override;

 private:
  friend class ContextGLES;

  std::vector<std::pair<uint64_t, std::shared_ptr<const Sampler>>> samplers_;

  SamplerLibraryGLES();

  // |SamplerLibrary|
  raw_ptr<const Sampler> GetSampler(
      const SamplerDescriptor& descriptor) override;

  bool supports_decal_sampler_address_mode_ = false;

  SamplerLibraryGLES(const SamplerLibraryGLES&) = delete;

  SamplerLibraryGLES& operator=(const SamplerLibraryGLES&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_GLES_SAMPLER_LIBRARY_GLES_H_
