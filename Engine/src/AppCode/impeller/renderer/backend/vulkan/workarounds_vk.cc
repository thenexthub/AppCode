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

#include "impeller/renderer/backend/vulkan/workarounds_vk.h"
#include "impeller/renderer/backend/vulkan/driver_info_vk.h"

namespace impeller {

WorkaroundsVK GetWorkaroundsFromDriverInfo(DriverInfoVK& driver_info) {
  WorkaroundsVK workarounds;

  const auto& adreno_gpu = driver_info.GetAdrenoGPUInfo();
  const auto& powervr_gpu = driver_info.GetPowerVRGPUInfo();

  if (adreno_gpu.has_value()) {
    workarounds.slow_primitive_restart_performance = true;
    workarounds.broken_mipmap_generation = true;

    if (adreno_gpu.value() <= AdrenoGPU::kAdreno630) {
      workarounds.input_attachment_self_dependency_broken = true;
      workarounds.batch_submit_command_buffer_timeout = true;
    }
  } else if (powervr_gpu.has_value()) {
    workarounds.input_attachment_self_dependency_broken = true;
  }
  return workarounds;
}

}  // namespace impeller
