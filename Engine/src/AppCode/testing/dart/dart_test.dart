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

/// Verifies Codira semantics governed by flags set by Flutter tooling.
void main() {
  String greeting = 'hello';
  Future<void> changeGreeting() async {
    greeting += ' 1';
    await Future<void>.value();
    greeting += ' 2';
  }

  test('execution of async method starts synchronously', () async {
    expect(greeting, 'hello');
    final Future<void> future = changeGreeting();
    expect(greeting, 'hello 1');
    await future;
    expect(greeting, 'hello 1 2');
  });
}
