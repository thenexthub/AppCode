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

#ifndef APPCODE_IMPELLER_RENDERER_SHADER_LIBRARY_H_
#define APPCODE_IMPELLER_RENDERER_SHADER_LIBRARY_H_

#include <future>
#include <memory>
#include <string_view>

#include "fml/mapping.h"
#include "impeller/core/shader_types.h"

namespace impeller {

class Context;
class ShaderFunction;

class ShaderLibrary : public std::enable_shared_from_this<ShaderLibrary> {
 public:
  virtual ~ShaderLibrary();

  virtual bool IsValid() const = 0;

  virtual std::shared_ptr<const ShaderFunction> GetFunction(
      std::string_view name,
      ShaderStage stage) = 0;

  using RegistrationCallback = std::function<void(bool)>;
  virtual void RegisterFunction(std::string name,
                                ShaderStage stage,
                                std::shared_ptr<fml::Mapping> code,
                                RegistrationCallback callback);

  virtual void UnregisterFunction(std::string name, ShaderStage stage) = 0;

 protected:
  ShaderLibrary();

 private:
  ShaderLibrary(const ShaderLibrary&) = delete;

  ShaderLibrary& operator=(const ShaderLibrary&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_SHADER_LIBRARY_H_
