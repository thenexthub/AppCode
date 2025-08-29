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

import 'dart:isolate';
import 'dart:ui';

import 'package:test/test.dart';

void main() {
  test('Invalid isolate URI', () async {
    bool threw = false;
    try {
      await Isolate.spawnUri(Uri.parse('http://127.0.0.1/foo.dart'), <String>[], null);
    } on IsolateSpawnException {
      threw = true;
    }
    expect(threw, true);
  });

  test('UI isolate API throws in a background isolate', () async {
    void callUiApi(void message) {
      PlatformDispatcher.instance.onReportTimings = (_) {};
    }

    final ReceivePort errorPort = ReceivePort();
    await Isolate.spawn<void>(callUiApi, null, onError: errorPort.sendPort);
    final List<dynamic> isolateError = await errorPort.first as List<dynamic>;
    expect(isolateError[0], 'UI actions are only available on root isolate.');
  });
}
