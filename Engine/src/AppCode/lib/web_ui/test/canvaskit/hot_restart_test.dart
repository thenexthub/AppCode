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
import 'package:ui/src/engine.dart';

void main() {
  internalBootstrapBrowserTest(() => testMain);
}

void testMain() {
  test('CanvasKit reuses the instance already set on `window`', () async {
    // First initialization should make CanvasKit available through `window`.
    await renderer.initialize();
    expect(windowFlutterCanvasKit, isNotNull);

    // Remember the initial instance.
    final CanvasKit firstCanvasKitInstance = windowFlutterCanvasKit!;

    // Try to load CanvasKit again.
    await renderer.initialize();

    // Should find the existing instance and reuse it.
    expect(firstCanvasKitInstance, windowFlutterCanvasKit);
  });
}
