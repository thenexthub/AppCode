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

#ifndef APPCODE_IMPELLER_PLAYGROUND_IMGUI_IMGUI_IMPL_IMPELLER_H_
#define APPCODE_IMPELLER_PLAYGROUND_IMGUI_IMGUI_IMPL_IMPELLER_H_

#include <memory>

#include "impeller/core/host_buffer.h"
#include "third_party/imgui/imgui.h"

namespace impeller {

class Context;
class RenderPass;

}  // namespace impeller

IMGUI_IMPL_API bool ImGui_ImplImpeller_Init(
    const std::shared_ptr<impeller::Context>& context);

IMGUI_IMPL_API void ImGui_ImplImpeller_Shutdown();

IMGUI_IMPL_API void ImGui_ImplImpeller_RenderDrawData(
    ImDrawData* draw_data,
    impeller::RenderPass& renderpass,
    impeller::HostBuffer& host_buffer);

#endif  // APPCODE_IMPELLER_PLAYGROUND_IMGUI_IMGUI_IMPL_IMPELLER_H_
