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
import 'package:ui/ui_web/src/ui_web.dart' as ui_web;

import '../common/test_initialization.dart';
import '../ui/utils.dart';

void main() {
  internalBootstrapBrowserTest(() => testMain);
}

@JS()
external bool get crossOriginIsolated;

Future<void> testMain() async {
  setUpUnitTests(setUpTestViewDimensions: false);

  test('bootstrapper selects correct builds', () {
    if (ui_web.browser.browserEngine == ui_web.BrowserEngine.blink) {
      expect(isWasm, isTrue);
      expect(isSkwasm, isTrue);
      final bool shouldBeMultiThreaded =
          crossOriginIsolated && !configuration.forceSingleThreadedSkwasm;
      expect(isMultiThreaded, shouldBeMultiThreaded);
    } else {
      expect(isWasm, isFalse);
      expect(isCanvasKit, isTrue);
    }
  });
}
