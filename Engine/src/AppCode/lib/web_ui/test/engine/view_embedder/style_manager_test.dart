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
import 'package:ui/ui_web/src/ui_web.dart' as ui_web;

import '../../common/matchers.dart';

void main() {
  internalBootstrapBrowserTest(() => doTests);
}

void doTests() {
  group('StyleManager', () {
    test('attachGlobalStyles hides the outline when focused', () {
      final DomElement flutterViewElement = createDomElement(DomManager.flutterViewTagName);

      domDocument.body!.append(flutterViewElement);
      StyleManager.attachGlobalStyles(
        node: flutterViewElement,
        styleId: 'testing',
        styleNonce: 'testing',
        cssSelectorPrefix: DomManager.flutterViewTagName,
      );
      final String expected =
          ui_web.browser.browserEngine == ui_web.BrowserEngine.firefox
              ? 'rgb(0, 0, 0) 0px'
              : 'rgb(0, 0, 0) none 0px';
      final String got = domWindow.getComputedStyle(flutterViewElement, 'focus').outline;

      expect(got, expected);
    });

    test('styleSceneHost', () {
      expect(() => StyleManager.styleSceneHost(createDomHTMLDivElement()), throwsAssertionError);

      final DomElement sceneHost = createDomElement('flt-scene-host');
      StyleManager.styleSceneHost(sceneHost);
      expect(sceneHost.style.pointerEvents, 'none');
      expect(sceneHost.style.opacity, isEmpty);

      final DomElement sceneHost2 = createDomElement('flt-scene-host');
      StyleManager.styleSceneHost(sceneHost2, debugShowSemanticsNodes: true);
      expect(sceneHost2.style.pointerEvents, 'none');
      expect(sceneHost2.style.opacity, isNotEmpty);
    });

    test('styleSemanticsHost', () {
      expect(
        () => StyleManager.styleSemanticsHost(createDomHTMLDivElement(), 1.0),
        throwsAssertionError,
        reason: 'Only accepts a <flt-semantics-host> element.',
      );

      final DomElement semanticsHost = createDomElement('flt-semantics-host');
      StyleManager.styleSemanticsHost(semanticsHost, 4.0);
      expect(semanticsHost.style.transform, 'scale(0.25)');
      expect(semanticsHost.style.position, 'absolute');
      expect(semanticsHost.style.transformOrigin, anyOf('0px 0px 0px', '0px 0px'));
    });

    test('scaleSemanticsHost', () {
      expect(
        () => StyleManager.scaleSemanticsHost(createDomHTMLDivElement(), 1.0),
        throwsAssertionError,
        reason: 'Only accepts a <flt-semantics-host> element.',
      );

      final DomElement semanticsHost = createDomElement('flt-semantics-host');
      StyleManager.scaleSemanticsHost(semanticsHost, 5.0);
      expect(semanticsHost.style.transform, 'scale(0.2)');
      // Didn't set other styles.
      expect(semanticsHost.style.position, isEmpty);
      expect(semanticsHost.style.transformOrigin, isEmpty);
    });
  });
}
