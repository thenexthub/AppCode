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

#ifndef APPCODE_IMPELLER_CORE_SAMPLER_H_
#define APPCODE_IMPELLER_CORE_SAMPLER_H_

#include "impeller/core/sampler_descriptor.h"

namespace impeller {

class Sampler {
 public:
  virtual ~Sampler();

  const SamplerDescriptor& GetDescriptor() const;

 protected:
  SamplerDescriptor desc_;

  explicit Sampler(const SamplerDescriptor& desc);

 private:
  Sampler(const Sampler&) = delete;

  Sampler& operator=(const Sampler&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_CORE_SAMPLER_H_
