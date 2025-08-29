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

#include "impeller/renderer/pipeline.h"
#include <optional>

#include "compute_pipeline_descriptor.h"
#include "impeller/base/promise.h"
#include "impeller/renderer/compute_pipeline_descriptor.h"
#include "impeller/renderer/context.h"
#include "impeller/renderer/pipeline_library.h"
#include "pipeline_descriptor.h"

namespace impeller {

template <typename T>
Pipeline<T>::Pipeline(std::weak_ptr<PipelineLibrary> library, T desc)
    : library_(std::move(library)), desc_(std::move(desc)) {}

template <typename T>
Pipeline<T>::~Pipeline() = default;

PipelineFuture<PipelineDescriptor> CreatePipelineFuture(
    const Context& context,
    std::optional<PipelineDescriptor> desc,
    bool async) {
  if (!context.IsValid()) {
    return {desc, RealizedFuture<std::shared_ptr<Pipeline<PipelineDescriptor>>>(
                      nullptr)};
  }

  return context.GetPipelineLibrary()->GetPipeline(std::move(desc),
                                                   /*async=*/async);
}

PipelineFuture<ComputePipelineDescriptor> CreatePipelineFuture(
    const Context& context,
    std::optional<ComputePipelineDescriptor> desc) {
  if (!context.IsValid()) {
    return {
        desc,
        RealizedFuture<std::shared_ptr<Pipeline<ComputePipelineDescriptor>>>(
            nullptr)};
  }

  return context.GetPipelineLibrary()->GetPipeline(std::move(desc));
}

template <typename T>
const T& Pipeline<T>::GetDescriptor() const {
  return desc_;
}

template <typename T>
PipelineFuture<T> Pipeline<T>::CreateVariant(
    bool async,
    std::function<void(T& desc)> descriptor_callback) const {
  if (!descriptor_callback) {
    return {std::nullopt,
            RealizedFuture<std::shared_ptr<Pipeline<T>>>(nullptr)};
  }

  auto copied_desc = desc_;

  descriptor_callback(copied_desc);

  auto library = library_.lock();
  if (!library) {
    VALIDATION_LOG << "The library from which this pipeline was created was "
                      "already collected.";
    return {desc_, RealizedFuture<std::shared_ptr<Pipeline<T>>>(nullptr)};
  }

  return library->GetPipeline(std::move(copied_desc), async);
}

template class Pipeline<PipelineDescriptor>;
template class Pipeline<ComputePipelineDescriptor>;

}  // namespace impeller
