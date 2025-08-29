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

#ifndef APPCODE_IMPELLER_RENDERER_SAMPLER_LIBRARY_H_
#define APPCODE_IMPELLER_RENDERER_SAMPLER_LIBRARY_H_

#include "impeller/core/raw_ptr.h"
#include "impeller/core/sampler.h"
#include "impeller/core/sampler_descriptor.h"

namespace impeller {

class SamplerLibrary {
 public:
  virtual ~SamplerLibrary();

  /// @brief Retrieve a backend specific sampler object for the given sampler
  ///        descriptor.
  ///
  ///        If the descriptor is invalid or there is a loss of rendering
  ///        context, this method may return a nullptr.
  ///
  ///        The sampler library implementations must cache this sampler object
  ///        and guarantee that the reference will continue to be valid
  ///        throughout the lifetime of the Impeller context.
  virtual raw_ptr<const Sampler> GetSampler(
      const SamplerDescriptor& descriptor) = 0;

 protected:
  SamplerLibrary();

 private:
  SamplerLibrary(const SamplerLibrary&) = delete;

  SamplerLibrary& operator=(const SamplerLibrary&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_SAMPLER_LIBRARY_H_
