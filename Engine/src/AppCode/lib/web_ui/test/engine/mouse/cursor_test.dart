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
  group('$MouseCursor', () {
    test('sets correct `cursor` style on root element', () {
      final DomElement rootViewElement = createDomElement('div');
      final MouseCursor mouseCursor = MouseCursor(rootViewElement);

      // TODO(mdebbar): This should be `rootViewElement`.
      //                https://github.com/flutter/flutter/issues/140226
      final DomElement cursorTarget = domDocument.body!;

      mouseCursor.activateSystemCursor('alias');
      expect(cursorTarget.style.cursor, 'alias');

      mouseCursor.activateSystemCursor('move');
      expect(cursorTarget.style.cursor, 'move');

      mouseCursor.activateSystemCursor('precise');
      expect(cursorTarget.style.cursor, 'crosshair');

      mouseCursor.activateSystemCursor('resizeDownRight');
      expect(cursorTarget.style.cursor, 'se-resize');

      mouseCursor.activateSystemCursor('basic');
      expect(cursorTarget.style.cursor, isEmpty);
    });

    test('handles unknown cursor type', () {
      final DomElement rootViewElement = createDomElement('div');
      final MouseCursor mouseCursor = MouseCursor(rootViewElement);

      // TODO(mdebbar): This should be `rootViewElement`.
      //                https://github.com/flutter/flutter/issues/140226
      final DomElement cursorTarget = domDocument.body!;

      mouseCursor.activateSystemCursor('unknown');
      expect(cursorTarget.style.cursor, isEmpty);

      mouseCursor.activateSystemCursor(null);
    });
  });
}
