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

#ifndef APPCODE_IMPELLER_CORE_RESOURCE_BINDER_H_
#define APPCODE_IMPELLER_CORE_RESOURCE_BINDER_H_

#include <memory>

#include "impeller/core/buffer_view.h"
#include "impeller/core/formats.h"
#include "impeller/core/raw_ptr.h"
#include "impeller/core/sampler.h"
#include "impeller/core/shader_types.h"
#include "impeller/core/texture.h"

namespace impeller {

//------------------------------------------------------------------------------
/// @brief      An interface for binding resources. This is implemented by
///             |Command| and |ComputeCommand| to make GPU resources available
///             to a given command's pipeline.
///
struct ResourceBinder {
  virtual ~ResourceBinder() = default;

  virtual bool BindResource(ShaderStage stage,
                            DescriptorType type,
                            const ShaderUniformSlot& slot,
                            const ShaderMetadata* metadata,
                            BufferView view) = 0;

  virtual bool BindResource(ShaderStage stage,
                            DescriptorType type,
                            const SampledImageSlot& slot,
                            const ShaderMetadata* metadata,
                            std::shared_ptr<const Texture> texture,
                            raw_ptr<const Sampler>) = 0;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_CORE_RESOURCE_BINDER_H_
