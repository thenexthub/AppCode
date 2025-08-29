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
import 'dart:typed_data';

import 'package:test/bootstrap/browser.dart';
import 'package:test/test.dart';
import 'package:ui/src/engine.dart';
import 'package:ui/ui.dart' as ui;

import '../common/test_initialization.dart';

const MethodCodec codec = JSONMethodCodec();

EngineFlutterWindow get implicitView => EnginePlatformDispatcher.instance.implicitView!;

void main() {
  internalBootstrapBrowserTest(() => testMain);
}

void testMain() {
  group('without implicit view', () {
    test('Handles navigation gracefully when no implicit view exists', () async {
      expect(EnginePlatformDispatcher.instance.implicitView, isNull);

      final Completer<ByteData?> completer = Completer<ByteData?>();
      ui.PlatformDispatcher.instance.sendPlatformMessage(
        'flutter/navigation',
        codec.encodeMethodCall(
          const MethodCall('routeUpdated', <String, dynamic>{'routeName': '/foo'}),
        ),
        (ByteData? response) => completer.complete(response),
      );
      final ByteData? response = await completer.future;
      expect(response, isNull);
    });
  });

  group('with implicit view', () {
    late TestUrlStrategy strategy;

    setUpImplicitView();

    setUp(() async {
      strategy = TestUrlStrategy();
      await implicitView.debugInitializeHistory(strategy, useSingle: true);
    });

    test('Tracks pushed, replaced and popped routes', () async {
      final Completer<void> completer = Completer<void>();
      ui.PlatformDispatcher.instance.sendPlatformMessage(
        'flutter/navigation',
        codec.encodeMethodCall(
          const MethodCall('routeUpdated', <String, dynamic>{'routeName': '/foo'}),
        ),
        (_) => completer.complete(),
      );
      await completer.future;
      expect(strategy.getPath(), '/foo');
    });
  });
}
