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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_GLES_SAMPLER_GLES_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_GLES_SAMPLER_GLES_H_

#include "impeller/base/backend_cast.h"
#include "impeller/core/sampler.h"

namespace impeller {

class TextureGLES;
class SamplerLibraryGLES;
class ProcTableGLES;

class SamplerGLES final : public Sampler,
                          public BackendCast<SamplerGLES, Sampler> {
 public:
  ~SamplerGLES();

  bool ConfigureBoundTexture(const TextureGLES& texture,
                             const ProcTableGLES& gl) const;

 private:
  friend class SamplerLibraryGLES;

  explicit SamplerGLES(const SamplerDescriptor&);

  SamplerGLES(const SamplerGLES&) = delete;

  SamplerGLES& operator=(const SamplerGLES&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_GLES_SAMPLER_GLES_H_
