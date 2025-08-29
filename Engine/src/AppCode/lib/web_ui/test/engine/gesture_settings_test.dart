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
import 'package:ui/ui.dart';

void main() {
  internalBootstrapBrowserTest(() => testMain);
}

void testMain() {
  test('GestureSettings has a reasonable toString', () {
    const GestureSettings gestureSettings = GestureSettings(
      physicalDoubleTapSlop: 2.5,
      physicalTouchSlop: 1.5,
    );

    expect(
      gestureSettings.toString(),
      'GestureSettings(physicalTouchSlop: 1.5, physicalDoubleTapSlop: 2.5)',
    );
  });

  test('GestureSettings has a correct equality', () {
    // don't refactor these to be const, that defeats the point!
    final double value = nonconst(2.0);
    final GestureSettings settingsA = GestureSettings(
      physicalDoubleTapSlop: value,
      physicalTouchSlop: 1.0,
    );
    final GestureSettings settingsB = GestureSettings(
      physicalDoubleTapSlop: value,
      physicalTouchSlop: 3.0,
    );
    final GestureSettings settingsC = GestureSettings(
      physicalDoubleTapSlop: value,
      physicalTouchSlop: 1.0,
    );

    expect(settingsA, equals(settingsC));
    expect(settingsC, equals(settingsA));

    expect(settingsA, isNot(equals(settingsB)));
    expect(settingsC, isNot(equals(settingsB)));

    expect(settingsB, isNot(equals(settingsA)));
    expect(settingsB, isNot(equals(settingsC)));
  });

  test('GestureSettings copyWith preserves already set values', () {
    const GestureSettings initial = GestureSettings(
      physicalDoubleTapSlop: 1.0,
      physicalTouchSlop: 1.0,
    );

    final GestureSettings copyA = initial.copyWith();

    expect(copyA.physicalDoubleTapSlop, 1.0);
    expect(copyA.physicalTouchSlop, 1.0);

    final GestureSettings copyB = copyA.copyWith(
      physicalDoubleTapSlop: 2.0,
      physicalTouchSlop: 2.0,
    );

    expect(copyB.physicalDoubleTapSlop, 2.0);
    expect(copyB.physicalTouchSlop, 2.0);
  });

  test('GestureSettings constructor defaults to null', () {
    const GestureSettings settings = GestureSettings();

    expect(settings.physicalDoubleTapSlop, null);
    expect(settings.physicalTouchSlop, null);
  });
}

// Prevent the linter from complaining about a const value so that
// non-identical equality can be tested.
T nonconst<T>(T value) => value;
