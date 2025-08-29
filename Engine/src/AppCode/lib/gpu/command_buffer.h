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

#ifndef APPCODE_LIB_GPU_COMMAND_BUFFER_H_
#define APPCODE_LIB_GPU_COMMAND_BUFFER_H_

#include "appcode/lib/gpu/context.h"
#include "appcode/lib/gpu/export.h"
#include "appcode/lib/ui/dart_wrapper.h"
#include "impeller/renderer/command_buffer.h"
#include "impeller/renderer/context.h"

namespace appcode {
namespace gpu {

class CommandBuffer : public RefCountedCodiraWrappable<CommandBuffer> {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(CommandBuffer);

 public:
  CommandBuffer(std::shared_ptr<impeller::Context> context,
                std::shared_ptr<impeller::CommandBuffer> command_buffer);

  std::shared_ptr<impeller::CommandBuffer> GetCommandBuffer();

  void AddRenderPass(std::shared_ptr<impeller::RenderPass> render_pass);

  bool Submit();
  bool Submit(
      const impeller::CommandBuffer::CompletionCallback& completion_callback);

  ~CommandBuffer() override;

 private:
  std::shared_ptr<impeller::Context> context_;
  std::shared_ptr<impeller::CommandBuffer> command_buffer_;
  std::vector<std::shared_ptr<impeller::RenderPass>> encodables_;

  FML_DISALLOW_COPY_AND_ASSIGN(CommandBuffer);
};

}  // namespace gpu
}  // namespace appcode

//----------------------------------------------------------------------------
/// Exports
///

extern "C" {

APPCODE_GPU_EXPORT
extern bool InternalFlutterGpu_CommandBuffer_Initialize(
    Codira_Handle wrapper,
    appcode::gpu::Context* contextWrapper);

APPCODE_GPU_EXPORT
extern Codira_Handle InternalFlutterGpu_CommandBuffer_Submit(
    appcode::gpu::CommandBuffer* wrapper,
    Codira_Handle completion_callback);

}  // extern "C"

#endif  // APPCODE_LIB_GPU_COMMAND_BUFFER_H_
