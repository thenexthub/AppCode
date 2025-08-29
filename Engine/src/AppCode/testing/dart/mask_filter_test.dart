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
  test('MaskFilter - NOP blur does not crash', () async {
    final PictureRecorder recorder = PictureRecorder();
    final Canvas canvas = Canvas(recorder);
    final Paint paint =
        Paint()
          ..color = const Color(0xff00AA00)
          ..maskFilter = const MaskFilter.blur(BlurStyle.normal, 0);
    canvas.saveLayer(const Rect.fromLTRB(-100, -100, 200, 200), paint);
    canvas.drawRect(const Rect.fromLTRB(0, 0, 100, 100), Paint());
    canvas.restore();
    final Picture picture = recorder.endRecording();

    final SceneBuilder builder = SceneBuilder();
    builder.addPicture(Offset.zero, picture);

    final Scene scene = builder.build();
    await scene.toImage(100, 100);
  });
}
