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

// FlutterTesterOptions=--enable-serial-gc

import 'package:test/test.dart';

int use(List<int> a) {
  return a[0];
}

void main() {
  test('Serial GC option test ', () async {
    const bool threw = false;
    for (int i = 0; i < 100; i++) {
      final List<int> a = <int>[100];
      use(a);
    }
    expect(threw, false);
  });
}
