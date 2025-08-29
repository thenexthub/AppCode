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

#include "impeller/renderer/backend/gles/pipeline_gles.h"

namespace impeller {

PipelineGLES::PipelineGLES(std::shared_ptr<ReactorGLES> reactor,
                           std::weak_ptr<PipelineLibrary> library,
                           const PipelineDescriptor& desc,
                           std::shared_ptr<UniqueHandleGLES> handle)
    : Pipeline(std::move(library), desc),
      reactor_(std::move(reactor)),
      handle_(std::move(handle)),
      is_valid_(handle_->IsValid()) {
  if (is_valid_) {
    reactor_->SetDebugLabel(handle_->Get(), GetDescriptor().GetLabel());
  }
}

// |Pipeline|
PipelineGLES::~PipelineGLES() = default;

// |Pipeline|
bool PipelineGLES::IsValid() const {
  return is_valid_;
}

const HandleGLES& PipelineGLES::GetProgramHandle() const {
  return handle_->Get();
}

const std::shared_ptr<UniqueHandleGLES> PipelineGLES::GetSharedHandle() const {
  return handle_;
}

BufferBindingsGLES* PipelineGLES::GetBufferBindings() const {
  return buffer_bindings_.get();
}

bool PipelineGLES::BuildVertexDescriptor(const ProcTableGLES& gl,
                                         GLuint program) {
  if (buffer_bindings_) {
    return false;
  }
  auto vtx_desc = std::make_unique<BufferBindingsGLES>();
  if (!vtx_desc->RegisterVertexStageInput(
          gl, GetDescriptor().GetVertexDescriptor()->GetStageInputs(),
          GetDescriptor().GetVertexDescriptor()->GetStageLayouts())) {
    return false;
  }
  if (!vtx_desc->ReadUniformsBindings(gl, program)) {
    return false;
  }
  buffer_bindings_ = std::move(vtx_desc);
  return true;
}

[[nodiscard]] bool PipelineGLES::BindProgram() const {
  if (!handle_->IsValid()) {
    return false;
  }
  auto handle = reactor_->GetGLHandle(handle_->Get());
  if (!handle.has_value()) {
    return false;
  }
  reactor_->GetProcTable().UseProgram(handle.value());
  return true;
}

[[nodiscard]] bool PipelineGLES::UnbindProgram() const {
  if (reactor_) {
    reactor_->GetProcTable().UseProgram(0u);
  }
  return true;
}

}  // namespace impeller
