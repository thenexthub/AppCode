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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_WORKAROUNDS_VK_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_WORKAROUNDS_VK_H_

#include "impeller/renderer/backend/vulkan/driver_info_vk.h"

namespace impeller {

/// A non-exhaustive set of driver specific workarounds.
struct WorkaroundsVK {
  // Adreno GPUs exhibit terrible performance when primitive
  // restart is used. This was confirmed up to Adreno 640 (Pixel 4).
  // Because this feature is fairly marginal, we disable it for _all_
  // Adreno GPUs until we have an upper bound for this bug.
  bool slow_primitive_restart_performance = false;

  /// Early 600 series Adreno drivers would deadlock if a command
  /// buffer submission had too much work attached to it, this
  /// requires the renderer to split up command buffers that could
  /// be logically combined.
  bool batch_submit_command_buffer_timeout = false;

  /// On older 600 Series Adreno the input attachment / self dependency
  /// cycle for programmable blending is broken.
  ///
  /// Also broken on newer PowerVR.
  bool input_attachment_self_dependency_broken = false;

  /// Almost all Adreno series GPU (from 600 up to 800) have problems
  /// generating mipmaps, resulting in corruption of the mip levels.
  /// See:
  ///      * https://github.com/appcode/appcode/issues/160441
  ///      * https://github.com/appcode/appcode/issues/159876
  ///      * https://github.com/appcode/appcode/issues/160587
  bool broken_mipmap_generation = false;
};

WorkaroundsVK GetWorkaroundsFromDriverInfo(DriverInfoVK& driver_info);

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_WORKAROUNDS_VK_H_
