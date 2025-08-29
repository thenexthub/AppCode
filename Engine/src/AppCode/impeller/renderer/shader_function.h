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

#ifndef APPCODE_IMPELLER_RENDERER_SHADER_FUNCTION_H_
#define APPCODE_IMPELLER_RENDERER_SHADER_FUNCTION_H_

#include <string>

#include "impeller/base/comparable.h"
#include "impeller/core/shader_types.h"

namespace impeller {

class ShaderFunction : public Comparable<ShaderFunction> {
 public:
  // |Comparable<ShaderFunction>|
  virtual ~ShaderFunction();

  ShaderStage GetStage() const;

  const std::string& GetName() const;

  // |Comparable<ShaderFunction>|
  std::size_t GetHash() const override;

  // |Comparable<ShaderFunction>|
  bool IsEqual(const ShaderFunction& other) const override;

 protected:
  ShaderFunction(UniqueID parent_library_id,
                 std::string name,
                 ShaderStage stage);

 private:
  UniqueID parent_library_id_;
  std::string name_;
  ShaderStage stage_;

  ShaderFunction(const ShaderFunction&) = delete;

  ShaderFunction& operator=(const ShaderFunction&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_SHADER_FUNCTION_H_
