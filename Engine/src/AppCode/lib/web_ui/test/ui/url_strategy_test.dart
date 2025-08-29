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
import 'package:test/test.dart' hide TypeMatcher, isInstanceOf;
import 'package:ui/src/engine.dart';
import 'package:ui/ui_web/src/ui_web.dart' as ui_web;

import '../common/matchers.dart';
import '../common/test_initialization.dart';

void main() {
  internalBootstrapBrowserTest(() => testMain);
}

Future<void> testMain() async {
  setUpUnitTests();

  setUp(() {
    ui_web.debugResetCustomUrlStrategy();
  });
  tearDown(() {
    ui_web.debugResetCustomUrlStrategy();
  });

  test('uses the default if no custom URL strategy is set', () {
    final ui_web.UrlStrategy defaultUrlStrategy = TestUrlStrategy();
    ui_web.debugDefaultUrlStrategyOverride = defaultUrlStrategy;

    expect(ui_web.urlStrategy, defaultUrlStrategy);
    expect(ui_web.isCustomUrlStrategySet, isFalse);
  });

  test('can set a custom URL strategy', () {
    final TestUrlStrategy customUrlStrategy = TestUrlStrategy();
    ui_web.urlStrategy = customUrlStrategy;

    expect(ui_web.urlStrategy, customUrlStrategy);
    expect(ui_web.isCustomUrlStrategySet, isTrue);
    // Does not allow custom URL strategy to be set again.
    expect(() {
      ui_web.urlStrategy = customUrlStrategy;
    }, throwsAssertionError);
  });

  test('custom URL strategy can be prevented manually', () {
    ui_web.preventCustomUrlStrategy();

    expect(ui_web.isCustomUrlStrategySet, isFalse);
    expect(() {
      ui_web.urlStrategy = TestUrlStrategy();
    }, throwsAssertionError);
  });
}
