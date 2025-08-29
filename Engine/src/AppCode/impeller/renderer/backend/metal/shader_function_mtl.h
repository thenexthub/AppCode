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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_METAL_SHADER_FUNCTION_MTL_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_METAL_SHADER_FUNCTION_MTL_H_

#include <Metal/Metal.h>

#include "impeller/base/backend_cast.h"
#include "impeller/renderer/shader_function.h"

namespace impeller {

class ShaderFunctionMTL final
    : public ShaderFunction,
      public BackendCast<ShaderFunctionMTL, ShaderFunction> {
 public:
  // |ShaderFunction|
  ~ShaderFunctionMTL() override;

  id<MTLFunction> GetMTLFunction() const;

  using CompileCallback = std::function<void(id<MTLFunction>)>;

  void GetMTLFunctionSpecialized(const std::vector<Scalar>& constants,
                                 const CompileCallback& callback) const;

 private:
  friend class ShaderLibraryMTL;

  id<MTLFunction> function_ = nullptr;
  id<MTLLibrary> library_ = nullptr;

  ShaderFunctionMTL(UniqueID parent_library_id,
                    id<MTLFunction> function,
                    id<MTLLibrary> library,
                    std::string name,
                    ShaderStage stage);

  ShaderFunctionMTL(const ShaderFunctionMTL&) = delete;

  ShaderFunctionMTL& operator=(const ShaderFunctionMTL&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_METAL_SHADER_FUNCTION_MTL_H_
