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
import 'package:ui/ui.dart' as ui;

void main() {
  internalBootstrapBrowserTest(() => testMain);
}

void testMain() {
  group('EngineFlutterDisplay', () {
    test('overrides and restores devicePixelRatio', () {
      final EngineFlutterDisplay display = EngineFlutterDisplay(
        id: 0,
        size: const ui.Size(100.0, 100.0),
        refreshRate: 60.0,
      );

      final double originalDevicePixelRatio = display.devicePixelRatio;
      display.debugOverrideDevicePixelRatio(99.3);
      expect(display.devicePixelRatio, 99.3);

      display.debugOverrideDevicePixelRatio(null);
      expect(display.devicePixelRatio, originalDevicePixelRatio);
    });

    test('computes device pixel ratio using window.devicePixelRatio and visualViewport.scale', () {
      final EngineFlutterDisplay display = EngineFlutterDisplay(
        id: 0,
        size: const ui.Size(100.0, 100.0),
        refreshRate: 60.0,
      );
      final double windowDpr = domWindow.devicePixelRatio;
      final double visualViewportScale = domWindow.visualViewport!.scale!;
      expect(display.browserDevicePixelRatio, windowDpr * visualViewportScale);
    });
  });
}
