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

/// Shows a text that is shown in app extension.
class DarwinAppExtensionScenario extends Scenario {
  /// Creates the DarwinAppExtensionScenario scenario.
  DarwinAppExtensionScenario(super.view);

  // Semi-arbitrary.
  final double _screenWidth = 700;

  @override
  void onBeginFrame(Duration duration) {
    final SceneBuilder builder = SceneBuilder();
    final PictureRecorder recorder = PictureRecorder();
    final Canvas canvas = Canvas(recorder);

    final ParagraphBuilder paragraphBuilder =
        ParagraphBuilder(ParagraphStyle())
          ..pushStyle(TextStyle(fontSize: 80))
          ..addText('flutter Scenarios app extension.')
          ..pop();
    final Paragraph paragraph = paragraphBuilder.build();

    paragraph.layout(ParagraphConstraints(width: _screenWidth));

    canvas.drawParagraph(paragraph, const Offset(50, 80));
    final Picture picture = recorder.endRecording();

    builder.addPicture(Offset.zero, picture, willChangeHint: true);
    final Scene scene = builder.build();
    view.render(scene);
    scene.dispose();
  }
}
