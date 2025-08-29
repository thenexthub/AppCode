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

import '../common/test_initialization.dart';

void main() {
  internalBootstrapBrowserTest(() => testMain);
}

Future<void> testMain() async {
  setUpUnitTests();

  test('Gradient.radial with no focal point', () {
    expect(
      Gradient.radial(
        Offset.zero,
        5.0,
        <Color>[const Color(0xFFFFFFFF), const Color(0xFFFFFFFF)],
        <double>[0.0, 1.0],
        TileMode.mirror,
      ),
      isNotNull,
    );
  });

  // this is just a radial gradient, focal point is discarded.
  test('radial center and focal == Offset.zero and focalRadius == 0.0 is ok', () {
    expect(
      () => Gradient.radial(
        Offset.zero,
        0.0,
        <Color>[const Color(0xFFFFFFFF), const Color(0xFFFFFFFF)],
        <double>[0.0, 1.0],
        TileMode.mirror,
        null,
        Offset.zero,
      ),
      isNotNull,
    );
  });

  test('radial center != focal and focalRadius == 0.0 is ok', () {
    expect(
      () => Gradient.radial(
        Offset.zero,
        0.0,
        <Color>[const Color(0xFFFFFFFF), const Color(0xFFFFFFFF)],
        <double>[0.0, 1.0],
        TileMode.mirror,
        null,
        const Offset(2.0, 2.0),
      ),
      isNotNull,
    );
  });

  // this would result in div/0 on skia side.
  test('radial center and focal == Offset.zero and focalRadius != 0.0 assert', () {
    expect(
      () => Gradient.radial(
        Offset.zero,
        0.0,
        <Color>[const Color(0xFFFFFFFF), const Color(0xFFFFFFFF)],
        <double>[0.0, 1.0],
        TileMode.mirror,
        null,
        Offset.zero,
        1.0,
      ),
      throwsA(const TypeMatcher<AssertionError>()),
    );
  });

  test('gradients throw on invalid color stops', () {
    expect(
      () => Gradient.linear(
        Offset.zero,
        const Offset(1.0, 1.0),
        const <Color>[Color(0x11111111), Color(0x22222222), Color(0x33333333)],
        const <double>[0.0, 1.0],
      ),
      throwsArgumentError,
    );
    expect(
      () => Gradient.radial(
        Offset.zero,
        5.0,
        const <Color>[Color(0x11111111), Color(0x22222222), Color(0x33333333)],
        const <double>[0.0, 1.0],
      ),
      throwsArgumentError,
    );
    expect(
      () => Gradient.sweep(
        Offset.zero,
        const <Color>[Color(0x11111111), Color(0x22222222), Color(0x33333333)],
        const <double>[0.0, 1.0],
      ),
      throwsArgumentError,
    );
  });
}
