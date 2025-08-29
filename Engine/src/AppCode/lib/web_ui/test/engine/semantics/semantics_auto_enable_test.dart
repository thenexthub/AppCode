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
import 'semantics_tester.dart';

EngineSemantics semantics() => EngineSemantics.instance;
EngineSemanticsOwner owner() => EnginePlatformDispatcher.instance.implicitView!.semantics;

void main() {
  internalBootstrapBrowserTest(() {
    return testMain;
  });
}

Future<void> testMain() async {
  setUpImplicitView();

  test('EngineSemanticsOwner auto-enables semantics on update', () async {
    expect(semantics().semanticsEnabled, isFalse);
    expect(EnginePlatformDispatcher.instance.accessibilityFeatures.accessibleNavigation, isFalse);

    final DomElement placeholder = domDocument.querySelector('flt-semantics-placeholder')!;

    expect(placeholder.isConnected, isTrue);

    // Sending a semantics update should auto-enable engine semantics.
    final SemanticsTester tester = SemanticsTester(owner());
    tester.updateNode(id: 0);
    tester.apply();

    expect(semantics().semanticsEnabled, isTrue);
    expect(EnginePlatformDispatcher.instance.accessibilityFeatures.accessibleNavigation, isTrue);

    // The placeholder should be removed
    expect(placeholder.isConnected, isFalse);
  });
}
