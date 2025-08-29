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
  internalBootstrapBrowserTest(() => doTests);
}

void doTests() {
  group('GlobalHtmlAttributes', () {
    test('applies global attributes to the root and host elements', () {
      final DomElement hostElement = createDomElement('host-element');
      final DomElement rootElement = createDomElement('root-element');
      final GlobalHtmlAttributes globalHtmlAttributes = GlobalHtmlAttributes(
        rootElement: rootElement,
        hostElement: hostElement,
      );

      globalHtmlAttributes.applyAttributes(
        viewId: 123,
        rendererTag: 'canvaskit',
        buildMode: 'release',
      );

      expect(rootElement.getAttribute('flt-view-id'), '123');
      expect(hostElement.getAttribute('flt-renderer'), 'canvaskit');
      expect(hostElement.getAttribute('flt-build-mode'), 'release');
      expect(hostElement.getAttribute('spellcheck'), 'false');

      globalHtmlAttributes.applyAttributes(viewId: 456, rendererTag: 'skwasm', buildMode: 'debug');

      expect(rootElement.getAttribute('flt-view-id'), '456');
      expect(hostElement.getAttribute('flt-renderer'), 'skwasm');
      expect(hostElement.getAttribute('flt-build-mode'), 'debug');
      expect(hostElement.getAttribute('spellcheck'), 'false');
    });
  });
}
