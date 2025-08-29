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

import 'dart:typed_data';

import 'package:test/bootstrap/browser.dart';
import 'package:test/test.dart';

import 'package:ui/src/engine/vector_math.dart';

void main() {
  internalBootstrapBrowserTest(() => testMain);
}

void testMain() {
  test('toMatrix32', () {
    final List<double> data = <double>[1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16];
    final Float32List m32 = toMatrix32(Float64List.fromList(data));
    expect(m32, Float32List.fromList(data));
  });

  test('FastMatrix32.transform', () {
    final FastMatrix32 fast = FastMatrix32(
      Float32List.fromList(<double>[2, 1, 0, 0, 1, 3, 0, 0, 0, 0, 0, 0, 4, 5, 0, 1]),
    );
    fast.transform(6, 7);

    // Just make sure that the fast version produces a result consistent with
    // the slow version.
    final Matrix4 slow = Matrix4.fromFloat32List(fast.matrix);
    final Float32List slowTransformed = Float32List.fromList(<double>[6, 7, 0]);
    slow.transform3(slowTransformed);

    expect(fast.transformedX, slowTransformed[0]);
    expect(fast.transformedY, slowTransformed[1]);
  });
}
