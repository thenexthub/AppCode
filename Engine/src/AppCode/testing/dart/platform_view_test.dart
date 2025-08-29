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

import 'dart:ui';

import 'package:test/test.dart';

void main() {
  test('PlatformView layers do not emit errors from tester', () async {
    final SceneBuilder builder = SceneBuilder();
    builder.addPlatformView(1);
    PlatformDispatcher.instance.onBeginFrame = (Duration duration) {
      final Scene scene = builder.build();
      PlatformDispatcher.instance.implicitView!.render(scene);
      scene.dispose();
    };
    PlatformDispatcher.instance.scheduleFrame();
    // Test harness asserts that this does not emit an error from the shell logs.
  });
}
