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

void main() {
  internalBootstrapBrowserTest(() => testMain);
}

void testMain() {
  group('PlatformViewManager', () {
    const int viewId = 6;

    group('createPlatformViewSlot', () {
      test('can render slot, even for views that might have never been rendered before', () async {
        final DomElement slot = createPlatformViewSlot(viewId);
        expect(slot, isNotNull);
        expect(slot.querySelector('slot'), isNotNull);
      });

      test('rendered markup contains required attributes', () async {
        final DomElement slot = createPlatformViewSlot(viewId);
        expect(
          slot.style.pointerEvents,
          'auto',
          reason: 'Should re-enable pointer events for the contents of the view.',
        );
        final DomElement innerSlot = slot.querySelector('slot')!;
        expect(
          innerSlot.getAttribute('name'),
          contains('$viewId'),
          reason: 'The name attribute of the inner SLOT tag must refer to the viewId.',
        );
      });
    });
  });

  test('getPlatformViewSlotName', () {
    expect(getPlatformViewSlotName(42), 'flt-pv-slot-42');
  });

  test('getPlatformViewDomId', () {
    expect(getPlatformViewDomId(42), 'flt-pv-42');
  });
}
