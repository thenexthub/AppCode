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

import 'dart:js_interop';
import 'dart:js_interop_unsafe';

import 'package:test/bootstrap/browser.dart';
import 'package:test/test.dart';
import 'package:ui/src/engine.dart';

void main() {
  internalBootstrapBrowserTest(() => testMain);
}

void testMain() {
  group('initializeEngineServices', () {
    test('stores user configuration', () async {
      final JsFlutterConfiguration config = JsFlutterConfiguration();
      config
        // `canvasKitBaseUrl` is required for the test to actually run.
        ..['canvasKitBaseUrl'] = '/canvaskit/'.toJS
        // A property under test, that we'll try to read later.
        ..['nonce'] = 'some_nonce'.toJS
        // A non-existing property to verify our js-interop doesn't crash.
        ..['nonexistentProperty'] = 32.0.toJS;

      // Remove window.flutterConfiguration (if it's there)
      config['flutterConfiguration'] = null;

      // TODO(web): Replace the above nullification by the following assertion
      // when wasm and JS tests initialize their config the same way:
      // assert(js_util.getProperty<Object?>(domWindow, 'flutterConfiguration') == null);

      await initializeEngineServices(jsConfiguration: config);

      expect(configuration.nonce, 'some_nonce');
    });
  });
}
