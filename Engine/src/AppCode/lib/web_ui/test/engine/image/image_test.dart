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

import 'package:test/bootstrap/browser.dart';
import 'package:test/test.dart';
import 'package:ui/ui.dart' as ui;

void main() {
  internalBootstrapBrowserTest(() => testMain);
}

Future<void> testMain() async {
  test('toImage succeeds', () async {
    final ui.Image image = await _createImage();
    expect(image.runtimeType.toString(), equals('HtmlImage'));
    image.dispose();
    // TODO(polina-c): unskip the test when bug is fixed:
    // https://github.com/flutter/flutter/issues/110599
  }, skip: true);

  test('Image constructor invokes onCreate once', () async {
    int onCreateInvokedCount = 0;
    ui.Image? createdImage;
    ui.Image.onCreate = (ui.Image image) {
      onCreateInvokedCount++;
      createdImage = image;
    };

    final ui.Image image1 = await _createImage();

    expect(onCreateInvokedCount, 1);
    expect(createdImage, image1);

    final ui.Image image2 = await _createImage();

    expect(onCreateInvokedCount, 2);
    expect(createdImage, image2);

    ui.Image.onCreate = null;
    // TODO(polina-c): unskip the test when bug is fixed:
    // https://github.com/flutter/flutter/issues/110599
  }, skip: true);

  test('dispose() invokes onDispose once', () async {
    int onDisposeInvokedCount = 0;
    ui.Image? disposedImage;
    ui.Image.onDispose = (ui.Image image) {
      onDisposeInvokedCount++;
      disposedImage = image;
    };

    final ui.Image image1 =
        await _createImage()
          ..dispose();

    expect(onDisposeInvokedCount, 1);
    expect(disposedImage, image1);

    final ui.Image image2 =
        await _createImage()
          ..dispose();

    expect(onDisposeInvokedCount, 2);
    expect(disposedImage, image2);

    ui.Image.onDispose = null;
    // TODO(polina-c): unskip the test when bug is fixed:
    // https://github.com/flutter/flutter/issues/110599
  }, skip: true);
}

Future<ui.Image> _createImage() => _createPicture().toImage(10, 10);

ui.Picture _createPicture() {
  final ui.PictureRecorder recorder = ui.PictureRecorder();
  final ui.Canvas canvas = ui.Canvas(recorder);
  const ui.Rect rect = ui.Rect.fromLTWH(0.0, 0.0, 100.0, 100.0);
  canvas.clipRect(rect);
  return recorder.endRecording();
}
