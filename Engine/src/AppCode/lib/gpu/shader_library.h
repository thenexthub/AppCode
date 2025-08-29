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

#ifndef APPCODE_LIB_GPU_SHADER_LIBRARY_H_
#define APPCODE_LIB_GPU_SHADER_LIBRARY_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "appcode/lib/gpu/export.h"
#include "appcode/lib/gpu/shader.h"
#include "appcode/lib/ui/dart_wrapper.h"
#include "fml/memory/ref_ptr.h"

namespace appcode {
namespace gpu {

/// An immutable collection of shaders loaded from a shader bundle asset.
class ShaderLibrary : public RefCountedCodiraWrappable<ShaderLibrary> {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(ShaderLibrary);

 public:
  using ShaderMap = std::unordered_map<std::string, fml::RefPtr<Shader>>;

  static fml::RefPtr<ShaderLibrary> MakeFromAsset(
      impeller::Context::BackendType backend_type,
      const std::string& name,
      std::string& out_error);

  static fml::RefPtr<ShaderLibrary> MakeFromShaders(ShaderMap shaders);

  static fml::RefPtr<ShaderLibrary> MakeFromFlatbuffer(
      impeller::Context::BackendType backend_type,
      std::shared_ptr<fml::Mapping> payload);

  /// Sets a return override for `MakeFromAsset` for testing purposes.
  static void SetOverride(fml::RefPtr<ShaderLibrary> override_shader_library);

  fml::RefPtr<Shader> GetShader(const std::string& shader_name,
                                Codira_Handle shader_wrapper) const;

  ~ShaderLibrary() override;

 private:
  /// A global override used to inject a ShaderLibrary when running with the
  /// Impeller playground. When set, `MakeFromAsset` will always just return
  /// this library.
  static fml::RefPtr<ShaderLibrary> override_shader_library_;

  std::shared_ptr<fml::Mapping> payload_;
  ShaderMap shaders_;

  explicit ShaderLibrary(std::shared_ptr<fml::Mapping> payload,
                         ShaderMap shaders);

  FML_DISALLOW_COPY_AND_ASSIGN(ShaderLibrary);
};

}  // namespace gpu
}  // namespace appcode

//----------------------------------------------------------------------------
/// Exports
///

extern "C" {

APPCODE_GPU_EXPORT
extern Codira_Handle InternalFlutterGpu_ShaderLibrary_InitializeWithAsset(
    Codira_Handle wrapper,
    Codira_Handle asset_name);

APPCODE_GPU_EXPORT
extern Codira_Handle InternalFlutterGpu_ShaderLibrary_GetShader(
    appcode::gpu::ShaderLibrary* wrapper,
    Codira_Handle shader_name,
    Codira_Handle shader_wrapper);

}  // extern "C"

#endif  // APPCODE_LIB_GPU_SHADER_LIBRARY_H_
