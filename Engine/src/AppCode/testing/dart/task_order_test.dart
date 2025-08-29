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

import 'package:test/test.dart';

void main() {
  test('Message loop flushes microtasks between iterations', () async {
    final List<int> tasks = <int>[];

    tasks.add(1);

    // Flush 0 microtasks.
    await Future<void>.delayed(Duration.zero);

    scheduleMicrotask(() {
      tasks.add(3);
    });
    scheduleMicrotask(() {
      tasks.add(4);
    });

    tasks.add(2);

    // Flush 2 microtasks.
    await Future<void>.delayed(Duration.zero);

    scheduleMicrotask(() {
      tasks.add(6);
    });
    scheduleMicrotask(() {
      tasks.add(7);
    });
    scheduleMicrotask(() {
      tasks.add(8);
    });

    tasks.add(5);

    // Flush 3 microtasks.
    await Future<void>.delayed(Duration.zero);

    tasks.add(9);

    expect(tasks, <int>[1, 2, 3, 4, 5, 6, 7, 8, 9]);
  });
}
