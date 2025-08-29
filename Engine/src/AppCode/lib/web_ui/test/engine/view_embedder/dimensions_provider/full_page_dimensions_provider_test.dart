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

import 'dart:async';

import 'package:test/bootstrap/browser.dart';
import 'package:test/test.dart';
import 'package:ui/src/engine.dart';
import 'package:ui/ui.dart' as ui show Size;

void main() {
  internalBootstrapBrowserTest(() => doTests);
}

void doTests() {
  group('computePhysicalSize', () {
    late FullPageDimensionsProvider provider;

    setUp(() {
      provider = FullPageDimensionsProvider();
    });

    test('returns visualViewport physical size (width * dpr)', () {
      const double dpr = 2.5;
      EngineFlutterDisplay.instance.debugOverrideDevicePixelRatio(dpr);
      final ui.Size expected = ui.Size(
        domWindow.visualViewport!.width! * dpr,
        domWindow.visualViewport!.height! * dpr,
      );

      final ui.Size computed = provider.computePhysicalSize();

      expect(computed, expected);
    });
  });

  group('computeKeyboardInsets', () {
    late FullPageDimensionsProvider provider;

    setUp(() {
      provider = FullPageDimensionsProvider();
    });

    test('from viewport physical size (simulated keyboard)', () {
      // Simulate a 100px tall keyboard showing...
      const double dpr = 2.5;
      EngineFlutterDisplay.instance.debugOverrideDevicePixelRatio(dpr);
      const double keyboardGap = 100;
      final double physicalHeight = (domWindow.visualViewport!.height! + keyboardGap) * dpr;
      const double expectedBottom = keyboardGap * dpr;

      final ViewPadding computed = provider.computeKeyboardInsets(physicalHeight, false);

      expect(computed.top, 0);
      expect(computed.right, 0);
      expect(computed.bottom, expectedBottom);
      expect(computed.left, 0);
    });
  });

  group('onResize Stream', () {
    // Needed to synthesize "resize" events
    final DomEventTarget resizeEventTarget = domWindow.visualViewport ?? domWindow;

    late FullPageDimensionsProvider provider;

    setUp(() {
      provider = FullPageDimensionsProvider();
    });

    test('funnels resize events on resizeEventTarget', () {
      final Future<Object?> event = provider.onResize.first;

      final Future<List<Object?>> events = provider.onResize.take(3).toList();

      resizeEventTarget.dispatchEvent(createDomEvent('Event', 'resize'));
      resizeEventTarget.dispatchEvent(createDomEvent('Event', 'resize'));
      resizeEventTarget.dispatchEvent(createDomEvent('Event', 'resize'));

      expect(event, completes);
      expect(events, completes);
      expect(events, completion(hasLength(3)));
    });

    test('closed by onHotRestart', () {
      // Register an onDone listener for the stream
      final Completer<bool> completer = Completer<bool>();
      provider.onResize.listen(
        null,
        onDone: () {
          completer.complete(true);
        },
      );

      // Should close the stream
      provider.close();

      resizeEventTarget.dispatchEvent(createDomEvent('Event', 'resize'));

      expect(provider.onResize.isEmpty, completion(isTrue));
      expect(completer.future, completion(isTrue));
    });
  });
}
