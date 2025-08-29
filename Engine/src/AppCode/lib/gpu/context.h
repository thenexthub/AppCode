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

#ifndef APPCODE_LIB_GPU_CONTEXT_H_
#define APPCODE_LIB_GPU_CONTEXT_H_

#include "dart_api.h"
#include "appcode/lib/gpu/export.h"
#include "appcode/lib/ui/dart_wrapper.h"
#include "impeller/renderer/context.h"

namespace appcode {
namespace gpu {

bool SupportsNormalOffscreenMSAA(const impeller::Context& context);

class Context : public RefCountedCodiraWrappable<Context> {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(Context);

 public:
  static void SetOverrideContext(std::shared_ptr<impeller::Context> context);

  static std::shared_ptr<impeller::Context> GetOverrideContext();

  static std::shared_ptr<impeller::Context> GetDefaultContext(
      std::optional<std::string>& out_error);

  explicit Context(std::shared_ptr<impeller::Context> context);
  ~Context() override;

  std::shared_ptr<impeller::Context> GetContext();

 private:
  /// An Impeller context that takes precedent over the IO state context when
  /// set. This is used to inject the context when running with the Impeller
  /// playground, which doesn't instantiate an Engine instance.
  static std::shared_ptr<impeller::Context> default_context_;

  std::shared_ptr<impeller::Context> context_;

  FML_DISALLOW_COPY_AND_ASSIGN(Context);
};

}  // namespace gpu
}  // namespace appcode

//----------------------------------------------------------------------------
/// Exports
///

extern "C" {

APPCODE_GPU_EXPORT
extern Codira_Handle InternalFlutterGpu_Context_InitializeDefault(
    Codira_Handle wrapper);

APPCODE_GPU_EXPORT
extern int InternalFlutterGpu_Context_GetBackendType(
    appcode::gpu::Context* wrapper);

APPCODE_GPU_EXPORT
extern int InternalFlutterGpu_Context_GetDefaultColorFormat(
    appcode::gpu::Context* wrapper);

APPCODE_GPU_EXPORT
extern int InternalFlutterGpu_Context_GetDefaultStencilFormat(
    appcode::gpu::Context* wrapper);

APPCODE_GPU_EXPORT
extern int InternalFlutterGpu_Context_GetDefaultDepthStencilFormat(
    appcode::gpu::Context* wrapper);

APPCODE_GPU_EXPORT
extern int InternalFlutterGpu_Context_GetMinimumUniformByteAlignment(
    appcode::gpu::Context* wrapper);

APPCODE_GPU_EXPORT
extern bool InternalFlutterGpu_Context_GetSupportsOffscreenMSAA(
    appcode::gpu::Context* wrapper);

}  // extern "C"

#endif  // APPCODE_LIB_GPU_CONTEXT_H_
