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

void main() {
  internalBootstrapBrowserTest(() => doTests);
}

void doTests() {
  final DomEventTarget eventTarget = createDomElement('div');

  group('dprChanged Stream', () {
    late DisplayDprStream dprStream;

    setUp(() async {
      dprStream = DisplayDprStream(
        EngineFlutterDisplay.instance,
        overrides: DebugDisplayDprStreamOverrides(getMediaQuery: (_) => eventTarget),
      );
    });

    test('funnels display DPR on every mediaQuery "change" event.', () async {
      final Future<List<double>> dprs =
          dprStream.dprChanged.take(3).timeout(const Duration(seconds: 1)).toList();

      // Simulate the events
      EngineFlutterDisplay.instance.debugOverrideDevicePixelRatio(6.9);
      eventTarget.dispatchEvent(createDomEvent('Event', 'change'));
      EngineFlutterDisplay.instance.debugOverrideDevicePixelRatio(4.2);
      eventTarget.dispatchEvent(createDomEvent('Event', 'change'));
      EngineFlutterDisplay.instance.debugOverrideDevicePixelRatio(0.71);
      eventTarget.dispatchEvent(createDomEvent('Event', 'change'));

      expect(await dprs, <double>[6.9, 4.2, 0.71]);
    });
  });
}
