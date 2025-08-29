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

import 'dart:js_interop';

import 'package:test/bootstrap/browser.dart';
import 'package:test/test.dart';
import 'package:ui/src/engine.dart';

import 'common.dart';

void main() {
  internalBootstrapBrowserTest(() => testMain);
}

// This test exists to make sure we don't accidentally run the CanvasKit test suite
// in "auto" mode. The CanvasKit variant should always be deterministic.
void testMain() {
  setUpCanvasKitTest();

  test('CanvasKit tests always run with a specific variant', () {
    expect(
      configuration.canvasKitVariant,
      anyOf(CanvasKitVariant.chromium, CanvasKitVariant.full),
      reason: 'canvasKitVariant must be set to "chromium" or "full" in canvaskit tests!',
    );
  });

  test('debugOverrideJsConfiguration can bypass (and restore) variant', () {
    // Set-up in the test_platform.dart file. See `_testBootstrapHandler`.
    final CanvasKitVariant originalValue = configuration.canvasKitVariant;
    expect(configuration.canvasKitVariant, isNot(CanvasKitVariant.auto));

    debugOverrideJsConfiguration(
      <String, Object?>{'canvasKitVariant': 'auto'}.jsify() as JsFlutterConfiguration?,
    );
    expect(configuration.canvasKitVariant, CanvasKitVariant.auto);

    debugOverrideJsConfiguration(null);
    expect(configuration.canvasKitVariant, originalValue);
  });
}
