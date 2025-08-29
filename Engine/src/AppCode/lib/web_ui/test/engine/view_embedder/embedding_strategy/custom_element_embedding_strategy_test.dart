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

@TestOn('browser')
library;

import 'package:test/bootstrap/browser.dart';
import 'package:test/test.dart';
import 'package:ui/src/engine/dom.dart';
import 'package:ui/src/engine/view_embedder/embedding_strategy/custom_element_embedding_strategy.dart';

void main() {
  internalBootstrapBrowserTest(() => doTests);
}

void doTests() {
  late CustomElementEmbeddingStrategy strategy;
  late DomElement target;

  group('initialize', () {
    setUp(() {
      target = createDomElement('this-is-the-target');
      domDocument.body!.append(target);
      strategy = CustomElementEmbeddingStrategy(target);
    });

    tearDown(() {
      target.remove();
    });

    test('Prepares target environment', () {
      expect(
        target.getAttribute('flt-embedding'),
        'custom-element',
        reason: 'Should identify itself as a specific key=value into the target element.',
      );
    });
  });

  group('attachViewRoot', () {
    setUp(() {
      target = createDomElement('this-is-the-target');
      domDocument.body!.append(target);
      strategy = CustomElementEmbeddingStrategy(target);
    });

    tearDown(() {
      target.remove();
    });

    test('Should attach glasspane into embedder target (body)', () async {
      final DomElement glassPane = createDomElement('some-tag-for-tests');
      final DomCSSStyleDeclaration style = glassPane.style;

      expect(glassPane.isConnected, isFalse);
      expect(style.position, '', reason: 'Should not have any specific position.');
      expect(style.width, '', reason: 'Should not have any size set.');

      strategy.attachViewRoot(glassPane);

      // Assert injection into <body>
      expect(glassPane.isConnected, isTrue, reason: 'Should inject glassPane into the document.');
      expect(glassPane.parent, target, reason: 'Should inject glassPane into the target element');

      final DomCSSStyleDeclaration styleAfter = glassPane.style;

      // Assert required styling to cover the viewport
      expect(styleAfter.position, 'relative', reason: 'Should be relatively positioned.');
      expect(styleAfter.display, 'block', reason: 'Should be display:block.');
      expect(styleAfter.width, '100%', reason: 'Should take 100% of the available width');
      expect(styleAfter.height, '100%', reason: 'Should take 100% of the available height');
      expect(
        styleAfter.overflow,
        'hidden',
        reason: 'Should hide the occasional oversized canvas elements.',
      );
      expect(
        styleAfter.touchAction,
        'none',
        reason: 'Should disable browser handling of touch events.',
      );
    });
  });
}
