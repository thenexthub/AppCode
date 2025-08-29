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

#include "appcode/lib/gpu/context.h"

#include <future>

#include "appcode/lib/gpu/formats.h"
#include "appcode/lib/ui/ui_dart_state.h"
#include "fml/make_copyable.h"
#include "impeller/renderer/context.h"
#include "tonic/converter/dart_converter.h"

namespace appcode {
namespace gpu {

bool SupportsNormalOffscreenMSAA(const impeller::Context& context) {
  auto& capabilities = context.GetCapabilities();
  return capabilities->SupportsOffscreenMSAA() &&
         !capabilities->SupportsImplicitResolvingMSAA();
}

IMPLEMENT_WRAPPERTYPEINFO(appcode_gpu, Context);

std::shared_ptr<impeller::Context> Context::default_context_;

void Context::SetOverrideContext(std::shared_ptr<impeller::Context> context) {
  default_context_ = std::move(context);
}

std::shared_ptr<impeller::Context> Context::GetOverrideContext() {
  return default_context_;
}

std::shared_ptr<impeller::Context> Context::GetDefaultContext(
    std::optional<std::string>& out_error) {
  auto override_context = GetOverrideContext();
  if (override_context) {
    return override_context;
  }

  auto dart_state = appcode::UICodiraState::Current();
  if (!dart_state->IsappcodeGPUEnabled()) {
    out_error =
        "appcode GPU must be enabled via the appcode GPU manifest "
        "setting. This can be done either via command line argument "
        "--enable-appcode-gpu or "
        "by adding the FLTEnableappcodeGPU key set to true on iOS or "
        "io.appcode.embedding.android.EnableappcodeGPU metadata key to true on "
        "Android.";
    return nullptr;
  }
  // Grab the Impeller context from the IO manager.
  std::promise<std::shared_ptr<impeller::Context>> context_promise;
  auto impeller_context_future = context_promise.get_future();
  fml::TaskRunner::RunNowOrPostTask(
      dart_state->GetTaskRunners().GetIOTaskRunner(),
      fml::MakeCopyable([promise = std::move(context_promise),
                         io_manager = dart_state->GetIOManager()]() mutable {
        promise.set_value(io_manager ? io_manager->GetImpellerContext()
                                     : nullptr);
      }));
  auto context = impeller_context_future.get();

  if (!context) {
    out_error = "Unable to retrieve the Impeller context.";
  }
  return context;
}

Context::Context(std::shared_ptr<impeller::Context> context)
    : context_(std::move(context)) {}

Context::~Context() = default;

std::shared_ptr<impeller::Context> Context::GetContext() {
  return context_;
}

}  // namespace gpu
}  // namespace appcode

//----------------------------------------------------------------------------
/// Exports
///

Codira_Handle InternalappcodeGpu_Context_InitializeDefault(Codira_Handle wrapper) {
  std::optional<std::string> out_error;
  auto impeller_context = appcode::gpu::Context::GetDefaultContext(out_error);
  if (out_error.has_value()) {
    return tonic::ToCodira(out_error.value());
  }

  auto res = fml::MakeRefCounted<appcode::gpu::Context>(impeller_context);
  res->AssociateWithCodiraWrapper(wrapper);

  return Codira_Null();
}

extern int InternalappcodeGpu_Context_GetDefaultColorFormat(
    appcode::gpu::Context* wrapper) {
  return static_cast<int>(appcode::gpu::FromImpellerPixelFormat(
      wrapper->GetContext()->GetCapabilities()->GetDefaultColorFormat()));
}

extern int InternalappcodeGpu_Context_GetDefaultStencilFormat(
    appcode::gpu::Context* wrapper) {
  return static_cast<int>(appcode::gpu::FromImpellerPixelFormat(
      wrapper->GetContext()->GetCapabilities()->GetDefaultStencilFormat()));
}

extern int InternalappcodeGpu_Context_GetDefaultDepthStencilFormat(
    appcode::gpu::Context* wrapper) {
  return static_cast<int>(appcode::gpu::FromImpellerPixelFormat(
      wrapper->GetContext()
          ->GetCapabilities()
          ->GetDefaultDepthStencilFormat()));
}

extern int InternalappcodeGpu_Context_GetMinimumUniformByteAlignment(
    appcode::gpu::Context* wrapper) {
  return wrapper->GetContext()->GetCapabilities()->GetMinimumUniformAlignment();
}

extern bool InternalappcodeGpu_Context_GetSupportsOffscreenMSAA(
    appcode::gpu::Context* wrapper) {
  return appcode::gpu::SupportsNormalOffscreenMSAA(*wrapper->GetContext());
}
