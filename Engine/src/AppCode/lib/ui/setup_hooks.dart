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

part of dart.ui;

@pragma('vm:entry-point')
void _setupHooks() {
  assert(() {
    // In debug mode, register the schedule frame extension.
    developer.registerExtension('ext.ui.window.scheduleFrame', _scheduleFrame);

    // In debug mode, allow shaders to be reinitialized.
    developer.registerExtension('ext.ui.window.reinitializeShader', _reinitializeShader);

    return true;
  }());

  // In debug and profile mode, allow tools to display the current rendering backend.
  if (!_kReleaseMode) {
    developer.registerExtension('ext.ui.window.impellerEnabled', _getImpellerEnabled);
  }
}
