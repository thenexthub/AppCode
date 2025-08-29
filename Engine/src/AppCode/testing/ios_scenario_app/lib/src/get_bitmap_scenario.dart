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

import 'scenario.dart';

/// A scenario with red on top and blue on the bottom.
class GetBitmapScenario extends Scenario {
  /// Creates the GetBitmap scenario.
  GetBitmapScenario(super.view);

  @override
  void onBeginFrame(Duration duration) {
    final PictureRecorder recorder = PictureRecorder();
    final Canvas canvas = Canvas(recorder);
    canvas.drawRect(
      Rect.fromLTWH(0, 0, view.physicalSize.width, 300),
      Paint()..color = const Color(0xFFFF0000),
    );
    canvas.drawRect(
      Rect.fromLTWH(0, view.physicalSize.height - 300, view.physicalSize.width, 300),
      Paint()..color = const Color(0xFF0000FF),
    );
    final Picture picture = recorder.endRecording();
    final SceneBuilder builder = SceneBuilder();
    builder.addPicture(Offset.zero, picture);
    final Scene scene = builder.build();
    view.render(scene);
    picture.dispose();
    scene.dispose();
  }
}
