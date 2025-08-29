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

#include "impeller/renderer/pipeline_library.h"

namespace impeller {

PipelineLibrary::PipelineLibrary() = default;

PipelineLibrary::~PipelineLibrary() = default;

PipelineFuture<PipelineDescriptor> PipelineLibrary::GetPipeline(
    std::optional<PipelineDescriptor> descriptor,
    bool async) {
  if (descriptor.has_value()) {
    return GetPipeline(descriptor.value(), async);
  }
  auto promise = std::make_shared<
      std::promise<std::shared_ptr<Pipeline<PipelineDescriptor>>>>();
  promise->set_value(nullptr);
  return {descriptor, promise->get_future()};
}

PipelineFuture<ComputePipelineDescriptor> PipelineLibrary::GetPipeline(
    std::optional<ComputePipelineDescriptor> descriptor,
    bool async) {
  if (descriptor.has_value()) {
    return GetPipeline(descriptor.value(), async);
  }
  auto promise = std::make_shared<
      std::promise<std::shared_ptr<Pipeline<ComputePipelineDescriptor>>>>();
  promise->set_value(nullptr);
  return {descriptor, promise->get_future()};
}

}  // namespace impeller
