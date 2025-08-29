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

#ifndef APPCODE_IMPELLER_COMPILER_COMPILER_BACKEND_H_
#define APPCODE_IMPELLER_COMPILER_COMPILER_BACKEND_H_

#include <cstdint>
#include <memory>
#include <variant>

#include "spirv_glsl.hpp"
#include "spirv_msl.hpp"
#include "spirv_sksl.h"

namespace impeller {
namespace compiler {

struct CompilerBackend {
  using MSLCompiler = std::shared_ptr<spirv_cross::CompilerMSL>;
  using GLSLCompiler = std::shared_ptr<spirv_cross::CompilerGLSL>;
  using SkSLCompiler = std::shared_ptr<CompilerSkSL>;
  using Compiler = std::variant<MSLCompiler, GLSLCompiler, SkSLCompiler>;

  enum class Type {
    kMSL,
    kGLSL,
    kGLSLVulkan,
    kSkSL,
  };

  explicit CompilerBackend(MSLCompiler compiler);

  explicit CompilerBackend(GLSLCompiler compiler);

  explicit CompilerBackend(SkSLCompiler compiler);

  CompilerBackend(Type type, Compiler compiler);

  CompilerBackend();

  ~CompilerBackend();

  Type GetType() const;

  const spirv_cross::Compiler* operator->() const;

  spirv_cross::Compiler* GetCompiler();

  explicit operator bool() const;

  enum class ExtendedResourceIndex {
    kPrimary,
    kSecondary,
  };
  uint32_t GetExtendedMSLResourceBinding(ExtendedResourceIndex index,
                                         spirv_cross::ID id) const;

  const spirv_cross::Compiler* GetCompiler() const;

 private:
  Type type_ = Type::kMSL;
  Compiler compiler_;

  const spirv_cross::CompilerMSL* GetMSLCompiler() const;

  const spirv_cross::CompilerGLSL* GetGLSLCompiler() const;

  const CompilerSkSL* GetSkSLCompiler() const;
};

}  // namespace compiler
}  // namespace impeller

#endif  // APPCODE_IMPELLER_COMPILER_COMPILER_BACKEND_H_
