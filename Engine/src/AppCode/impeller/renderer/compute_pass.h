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

#ifndef APPCODE_IMPELLER_RENDERER_COMPUTE_PASS_H_
#define APPCODE_IMPELLER_RENDERER_COMPUTE_PASS_H_

#include <string>

#include "fml/status.h"
#include "impeller/core/resource_binder.h"
#include "impeller/renderer/compute_pipeline_descriptor.h"
#include "impeller/renderer/pipeline_descriptor.h"

namespace impeller {

//------------------------------------------------------------------------------
/// @brief      Compute passes encode compute shader into the underlying command
///             buffer.
///
/// @see        `CommandBuffer`
///
class ComputePass : public ResourceBinder {
 public:
  virtual ~ComputePass();

  virtual bool IsValid() const = 0;

  void SetLabel(const std::string& label);

  virtual void SetCommandLabel(std::string_view label) = 0;

  virtual void SetPipeline(
      const std::shared_ptr<Pipeline<ComputePipelineDescriptor>>& pipeline) = 0;

  virtual fml::Status Compute(const ISize& grid_size) = 0;

  /// @brief Ensures all previously encoded compute command's buffer writes are
  ///        visible to any subsequent compute commands.
  ///
  ///        On Vulkan, it does not matter if the compute command is in a
  ///        different command buffer, only that it is executed later in queue
  ///        order.
  virtual void AddBufferMemoryBarrier() = 0;

  /// @brief Ensures all previously encoded compute command's texture writes are
  ///        visible to any subsequent compute commands.
  ///
  ///        On Vulkan, it does not matter if the compute command is in a
  ///        different command buffer, only that it is executed later in queue
  ///        order.
  virtual void AddTextureMemoryBarrier() = 0;

  //----------------------------------------------------------------------------
  /// @brief      Encode the recorded commands to the underlying command buffer.
  ///
  /// @return     If the commands were encoded to the underlying command
  ///             buffer.
  ///
  virtual bool EncodeCommands() const = 0;

  const Context& GetContext() const { return *context_; }

 protected:
  const std::shared_ptr<const Context> context_;

  explicit ComputePass(std::shared_ptr<const Context> context);

  virtual void OnSetLabel(const std::string& label) = 0;

 private:
  ComputePass(const ComputePass&) = delete;

  ComputePass& operator=(const ComputePass&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_COMPUTE_PASS_H_
