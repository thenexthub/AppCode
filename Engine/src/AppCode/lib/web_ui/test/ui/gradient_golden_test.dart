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

import 'dart:math' as math;

import 'package:test/bootstrap/browser.dart';
import 'package:test/test.dart';
import 'package:ui/ui.dart';
import 'package:web_engine_tester/golden_tester.dart';

import '../common/test_initialization.dart';
import 'utils.dart';

void main() {
  internalBootstrapBrowserTest(() => testMain);
}

Future<void> testMain() async {
  setUpUnitTests(withImplicitView: true, setUpTestViewDimensions: false);

  const Rect region = Rect.fromLTWH(0, 0, 300, 300);

  group('Gradients', () {
    test('Using a linear gradient on a paint', () async {
      final PictureRecorder recorder = PictureRecorder();
      final Canvas canvas = Canvas(recorder, region);
      canvas.drawRect(
        const Rect.fromLTRB(50, 50, 250, 250),
        Paint()
          ..shader = Gradient.linear(
            const Offset(50, 50),
            const Offset(250, 250),
            <Color>[const Color(0xFFFF0000), const Color(0xFF00FF00), const Color(0xFF0000FF)],
            <double>[0.0, 0.5, 1.0],
          ),
      );

      await drawPictureUsingCurrentRenderer(recorder.endRecording());

      await matchGoldenFile('linear_gradient_paint.png', region: region);
    });

    test('Using a radial gradient on a paint', () async {
      final PictureRecorder recorder = PictureRecorder();
      final Canvas canvas = Canvas(recorder, region);
      canvas.drawRect(
        const Rect.fromLTRB(50, 50, 250, 250),
        Paint()
          ..shader = Gradient.radial(
            const Offset(150, 150),
            100,
            <Color>[const Color(0xFFFF0000), const Color(0xFF00FF00), const Color(0xFF0000FF)],
            <double>[0.0, 0.5, 1.0],
          ),
      );

      await drawPictureUsingCurrentRenderer(recorder.endRecording());

      await matchGoldenFile('radial_gradient_paint.png', region: region);
    });

    test('Using a conical gradient on a paint', () async {
      final PictureRecorder recorder = PictureRecorder();
      final Canvas canvas = Canvas(recorder, region);
      canvas.drawRect(
        const Rect.fromLTRB(50, 50, 250, 250),
        Paint()
          ..shader = Gradient.radial(
            const Offset(200, 200),
            100,
            <Color>[const Color(0xFFFF0000), const Color(0xFF00FF00), const Color(0xFF0000FF)],
            <double>[0.0, 0.5, 1.0],
            TileMode.clamp,
            null,
            const Offset(50, 50),
            5,
          ),
      );

      await drawPictureUsingCurrentRenderer(recorder.endRecording());

      await matchGoldenFile('conical_gradient_paint.png', region: region);
    });

    test('Using a sweep gradient on a paint', () async {
      final PictureRecorder recorder = PictureRecorder();
      final Canvas canvas = Canvas(recorder, region);
      canvas.drawRect(
        const Rect.fromLTRB(50, 50, 250, 250),
        Paint()
          ..shader = Gradient.sweep(
            const Offset(150, 150),
            <Color>[const Color(0xFFFF0000), const Color(0xFF00FF00), const Color(0xFF0000FF)],
            <double>[0.0, 0.5, 1.0],
            TileMode.clamp,
            math.pi / 3.0,
            4.0 * math.pi / 3.0,
          ),
      );

      await drawPictureUsingCurrentRenderer(recorder.endRecording());

      await matchGoldenFile('sweep_gradient_paint.png', region: region);
    });
  });
}
