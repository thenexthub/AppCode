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

#include "impeller/renderer/backend/metal/shader_function_mtl.h"

namespace impeller {

ShaderFunctionMTL::ShaderFunctionMTL(UniqueID parent_library_id,
                                     id<MTLFunction> function,
                                     id<MTLLibrary> library,
                                     std::string name,
                                     ShaderStage stage)
    : ShaderFunction(parent_library_id, std::move(name), stage),
      function_(function),
      library_(library) {}

ShaderFunctionMTL::~ShaderFunctionMTL() = default;

void ShaderFunctionMTL::GetMTLFunctionSpecialized(
    const std::vector<Scalar>& constants,
    const CompileCallback& callback) const {
  MTLFunctionConstantValues* constantValues =
      [[MTLFunctionConstantValues alloc] init];
  size_t index = 0;
  for (const auto value : constants) {
    Scalar copied_value = value;
    [constantValues setConstantValue:&copied_value
                                type:MTLDataTypeFloat
                             atIndex:index];
    index++;
  }
  CompileCallback callback_value = callback;
  [library_ newFunctionWithName:@(GetName().data())
                 constantValues:constantValues
              completionHandler:^(id<MTLFunction> _Nullable function,
                                  NSError* _Nullable error) {
                callback_value(function);
              }];
}

id<MTLFunction> ShaderFunctionMTL::GetMTLFunction() const {
  return function_;
}

}  // namespace impeller
