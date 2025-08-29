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

import 'channel_util.dart';
import 'scenario.dart';

/// Tries to draw darwin system font: CupertinoSystemDisplay, CupertinoSystemText
class DarwinSystemFont extends Scenario {
  /// Creates the DarwinSystemFont scenario.
  DarwinSystemFont(super.view);

  // Semi-arbitrary.
  final double _screenWidth = 700;

  @override
  void onBeginFrame(Duration duration) {
    final SceneBuilder builder = SceneBuilder();
    final PictureRecorder recorder = PictureRecorder();
    final Canvas canvas = Canvas(recorder);

    final ParagraphBuilder paragraphBuilderDisplay =
        ParagraphBuilder(ParagraphStyle(fontFamily: 'CupertinoSystemDisplay'))
          ..pushStyle(TextStyle(fontSize: 50))
          ..addText('Cupertino System Display\n')
          ..pop();
    final ParagraphBuilder paragraphBuilderText =
        ParagraphBuilder(ParagraphStyle(fontFamily: 'CupertinoSystemText'))
          ..pushStyle(TextStyle(fontSize: 50))
          ..addText('Cupertino System Text\n')
          ..pop();

    final Paragraph paragraphPro = paragraphBuilderDisplay.build();
    paragraphPro.layout(ParagraphConstraints(width: _screenWidth));
    canvas.drawParagraph(paragraphPro, const Offset(50, 80));

    final Paragraph paragraphText = paragraphBuilderText.build();
    paragraphText.layout(ParagraphConstraints(width: _screenWidth));
    canvas.drawParagraph(paragraphText, const Offset(50, 200));

    final Picture picture = recorder.endRecording();

    builder.addPicture(Offset.zero, picture, willChangeHint: true);
    final Scene scene = builder.build();
    view.render(scene);
    scene.dispose();

    sendJsonMessage(
      dispatcher: view.platformDispatcher,
      channel: 'display_data',
      json: <String, dynamic>{'data': 'ready'},
    );
  }
}
