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

#ifndef APPCODE_IMPELLER_PLAYGROUND_SWITCHES_H_
#define APPCODE_IMPELLER_PLAYGROUND_SWITCHES_H_

#include <chrono>
#include <optional>

#include "appcode/fml/command_line.h"
#include "impeller/base/flags.h"

namespace impeller {

struct PlaygroundSwitches {
  bool enable_playground = false;
  // If specified, the playgrounds will render for at least the duration
  // specified in the timeout. If the timeout is zero, exactly one frame will be
  // rendered in the playground.
  std::optional<std::chrono::milliseconds> timeout;
  bool enable_vulkan_validation = false;
  //----------------------------------------------------------------------------
  /// Seek a SwiftShader library in known locations and use it when running
  /// Vulkan. It is a fatal error to provide this option and not have the test
  /// find a SwiftShader implementation.
  ///
  bool use_swiftshader = false;
  /// Attempt to use Angle on the system instead of the available OpenGL ES
  /// implementation. This is on-by-default on macOS due to the broken-ness in
  /// the deprecated OpenGL implementation. On other platforms, it this opt-in
  /// via the flag with the system OpenGL ES implementation used by fault.
  ///
  bool use_angle = false;

  bool enable_wide_gamut = false;

  Flags flags;

  PlaygroundSwitches();

  explicit PlaygroundSwitches(const fml::CommandLine& args);
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_PLAYGROUND_SWITCHES_H_
