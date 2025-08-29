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

import 'package:test/bootstrap/browser.dart';
import 'package:test/test.dart';

import 'package:ui/src/engine.dart';

import '../../common/test_initialization.dart';

EngineSemantics semantics() => EngineSemantics.instance;

void main() {
  internalBootstrapBrowserTest(() {
    return testMain;
  });
}

Future<void> testMain() async {
  setUpImplicitView();

  test('EngineSemantics is enabled via a placeholder click', () async {
    expect(semantics().semanticsEnabled, isFalse);

    // Synthesize a click on the placeholder.
    final DomElement placeholder = domDocument.querySelector('flt-semantics-placeholder')!;

    expect(placeholder.isConnected, isTrue);

    final DomRect rect = placeholder.getBoundingClientRect();
    placeholder.dispatchEvent(
      createDomMouseEvent('click', <Object?, Object?>{
        'clientX': (rect.left + (rect.right - rect.left) / 2).floor(),
        'clientY': (rect.top + (rect.bottom - rect.top) / 2).floor(),
      }),
    );

    // On mobile semantics is enabled asynchronously.
    if (isMobile) {
      while (placeholder.isConnected!) {
        await Future<void>.delayed(const Duration(milliseconds: 50));
      }
    }
    expect(semantics().semanticsEnabled, isTrue);
    expect(placeholder.isConnected, isFalse);
  });
}
