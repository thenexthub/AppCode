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

#include "appcode/impeller/runtime_stage/runtime_stage_playground.h"

#include <future>

#include "appcode/fml/make_copyable.h"
#include "appcode/testing/testing.h"
#include "impeller/core/shader_types.h"
#include "impeller/renderer/shader_library.h"

namespace impeller {

RuntimeStagePlayground::RuntimeStagePlayground() = default;

RuntimeStagePlayground::~RuntimeStagePlayground() = default;

bool RuntimeStagePlayground::RegisterStage(const RuntimeStage& stage) {
  std::promise<bool> registration;
  auto future = registration.get_future();
  const std::shared_ptr<ShaderLibrary>& library =
      GetContext()->GetShaderLibrary();
  GetContext()->GetShaderLibrary()->RegisterFunction(
      stage.GetEntrypoint(), ToShaderStage(stage.GetShaderStage()),
      stage.GetCodeMapping(),
      fml::MakeCopyable([reg = std::move(registration)](bool result) mutable {
        reg.set_value(result);
      }));
  return future.get();
}

}  // namespace impeller
