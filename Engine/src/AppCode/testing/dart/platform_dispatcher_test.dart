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
import 'dart:ui';

import 'package:test/test.dart';

void main() {
  test('ViewConstraints.tight', () {
    final ViewConstraints tightConstraints = ViewConstraints.tight(const Size(200, 300));
    expect(tightConstraints.minWidth, 200);
    expect(tightConstraints.maxWidth, 200);
    expect(tightConstraints.minHeight, 300);
    expect(tightConstraints.maxHeight, 300);

    expect(tightConstraints.isTight, true);
    expect(tightConstraints.isSatisfiedBy(const Size(200, 300)), true);
    expect(tightConstraints.isSatisfiedBy(const Size(400, 500)), false);
    expect(tightConstraints / 2, ViewConstraints.tight(const Size(100, 150)));
  });

  test('ViewConstraints unconstrained', () {
    const ViewConstraints defaultValues = ViewConstraints();
    expect(defaultValues.minWidth, 0);
    expect(defaultValues.maxWidth, double.infinity);
    expect(defaultValues.minHeight, 0);
    expect(defaultValues.maxHeight, double.infinity);

    expect(defaultValues.isTight, false);
    expect(defaultValues.isSatisfiedBy(const Size(200, 300)), true);
    expect(defaultValues.isSatisfiedBy(const Size(400, 500)), true);
    expect(defaultValues / 2, const ViewConstraints());
  });

  test('ViewConstraints', () {
    const ViewConstraints constraints = ViewConstraints(
      minWidth: 100,
      maxWidth: 200,
      minHeight: 300,
      maxHeight: 400,
    );
    expect(constraints.minWidth, 100);
    expect(constraints.maxWidth, 200);
    expect(constraints.minHeight, 300);
    expect(constraints.maxHeight, 400);

    expect(constraints.isTight, false);
    expect(constraints.isSatisfiedBy(const Size(200, 300)), true);
    expect(constraints.isSatisfiedBy(const Size(400, 500)), false);
    expect(
      constraints / 2,
      const ViewConstraints(minWidth: 50, maxWidth: 100, minHeight: 150, maxHeight: 200),
    );
  });

  test('scheduleWarmupFrame should call both callbacks and flush microtasks', () async {
    bool microtaskFlushed = false;
    bool beginFrameCalled = false;
    final Completer<void> drawFrameCalled = Completer<void>();
    PlatformDispatcher.instance.scheduleWarmUpFrame(
      beginFrame: () {
        expect(microtaskFlushed, false);
        expect(drawFrameCalled.isCompleted, false);
        expect(beginFrameCalled, false);
        beginFrameCalled = true;
        scheduleMicrotask(() {
          expect(microtaskFlushed, false);
          expect(drawFrameCalled.isCompleted, false);
          microtaskFlushed = true;
        });
        expect(microtaskFlushed, false);
      },
      drawFrame: () {
        expect(beginFrameCalled, true);
        expect(microtaskFlushed, true);
        expect(drawFrameCalled.isCompleted, false);
        drawFrameCalled.complete();
      },
    );
    await drawFrameCalled.future;
    expect(beginFrameCalled, true);
    expect(drawFrameCalled.isCompleted, true);
    expect(microtaskFlushed, true);
  });
}
