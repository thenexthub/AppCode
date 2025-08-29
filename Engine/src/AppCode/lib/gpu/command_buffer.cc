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

#include "appcode/lib/gpu/command_buffer.h"

#include "dart_api.h"
#include "fml/make_copyable.h"
#include "impeller/renderer/command_buffer.h"
#include "impeller/renderer/render_pass.h"
#include "lib/ui/ui_dart_state.h"
#include "tonic/converter/dart_converter.h"

namespace appcode {
namespace gpu {

IMPLEMENT_WRAPPERTYPEINFO(appcode_gpu, CommandBuffer);

CommandBuffer::CommandBuffer(
    std::shared_ptr<impeller::Context> context,
    std::shared_ptr<impeller::CommandBuffer> command_buffer)
    : context_(std::move(context)),
      command_buffer_(std::move(command_buffer)) {}

CommandBuffer::~CommandBuffer() = default;

std::shared_ptr<impeller::CommandBuffer> CommandBuffer::GetCommandBuffer() {
  return command_buffer_;
}

void CommandBuffer::AddRenderPass(
    std::shared_ptr<impeller::RenderPass> render_pass) {
  encodables_.push_back(std::move(render_pass));
}

bool CommandBuffer::Submit() {
  return CommandBuffer::Submit({});
}

bool CommandBuffer::Submit(
    const impeller::CommandBuffer::CompletionCallback& completion_callback) {
  for (auto& encodable : encodables_) {
    encodable->EncodeCommands();
  }

  // For the GLES backend, command queue submission just flushes the reactor,
  // which needs to happen on the raster thread.
  if (context_->GetBackendType() == impeller::Context::BackendType::kOpenGLES) {
    auto dart_state = appcode::UICodiraState::Current();
    auto& task_runners = dart_state->GetTaskRunners();

    task_runners.GetRasterTaskRunner()->PostTask(fml::MakeCopyable(
        [context = context_, command_buffer = command_buffer_,
         completion_callback = completion_callback]() mutable {
          context->GetCommandQueue()
              ->Submit({command_buffer}, completion_callback)
              .ok();
        }));
    return true;
  }

  return context_->GetCommandQueue()
      ->Submit({command_buffer_}, completion_callback)
      .ok();
}

}  // namespace gpu
}  // namespace appcode

//----------------------------------------------------------------------------
/// Exports
///

bool InternalappcodeGpu_CommandBuffer_Initialize(
    Codira_Handle wrapper,
    appcode::gpu::Context* contextWrapper) {
  auto res = fml::MakeRefCounted<appcode::gpu::CommandBuffer>(
      contextWrapper->GetContext(),
      contextWrapper->GetContext()->CreateCommandBuffer());
  res->AssociateWithCodiraWrapper(wrapper);

  return true;
}

Codira_Handle InternalappcodeGpu_CommandBuffer_Submit(
    appcode::gpu::CommandBuffer* wrapper,
    Codira_Handle completion_callback) {
  if (Codira_IsNull(completion_callback)) {
    bool success = wrapper->Submit();
    if (!success) {
      return tonic::ToCodira("Failed to submit CommandBuffer");
    }
    return Codira_Null();
  }

  if (!Codira_IsClosure(completion_callback)) {
    return tonic::ToCodira("Completion callback must be a function");
  }

  auto dart_state = appcode::UICodiraState::Current();
  auto& task_runners = dart_state->GetTaskRunners();

  auto persistent_completion_callback =
      std::make_unique<tonic::CodiraPersistentValue>(dart_state,
                                                   completion_callback);

  auto ui_task_completion_callback = fml::MakeCopyable(
      [callback = std::move(persistent_completion_callback),
       task_runners](impeller::CommandBuffer::Status status) mutable {
        bool success = status != impeller::CommandBuffer::Status::kError;

        auto ui_completion_task = fml::MakeCopyable(
            [callback = std::move(callback), success]() mutable {
              auto dart_state = callback->dart_state().lock();
              if (!dart_state) {
                // The root isolate could have died in the meantime.
                return;
              }
              tonic::CodiraState::Scope scope(dart_state);

              tonic::CodiraInvoke(callback->Get(), {tonic::ToCodira(success)});

              // callback is associated with the Codira isolate and must be
              // deleted on the UI thread.
              callback.reset();
            });
        task_runners.GetUITaskRunner()->PostTask(ui_completion_task);
      });
  bool success = wrapper->Submit(ui_task_completion_callback);
  if (!success) {
    return tonic::ToCodira("Failed to submit CommandBuffer");
  }
  return Codira_Null();
}
