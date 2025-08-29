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
import 'dart:convert';
import 'dart:developer' as developer;
import 'dart:typed_data';
import 'dart:ui';

import 'package:test/test.dart';
import 'package:vm_service/vm_service.dart' as vms;
import 'package:vm_service/vm_service_io.dart';

import '../impeller_enabled.dart';

void main() {
  test('Capture an SKP ', () async {
    final developer.ServiceProtocolInfo info = await developer.Service.getInfo();

    if (info.serverUri == null) {
      fail('This test must not be run with --disable-vm-service.');
    }

    final vms.VmService vmService = await vmServiceConnectUri(
      'ws://localhost:${info.serverUri!.port}${info.serverUri!.path}ws',
    );

    final Completer<void> completer = Completer<void>();
    PlatformDispatcher.instance.onBeginFrame = (Duration timeStamp) {
      final PictureRecorder recorder = PictureRecorder();
      final Canvas canvas = Canvas(recorder);
      canvas.drawRect(const Rect.fromLTRB(10, 10, 20, 20), Paint());
      final Picture picture = recorder.endRecording();

      final SceneBuilder builder = SceneBuilder();
      builder.addPicture(Offset.zero, picture);
      final Scene scene = builder.build();

      PlatformDispatcher.instance.implicitView!.render(scene);
      scene.dispose();
      completer.complete();
    };
    PlatformDispatcher.instance.scheduleFrame();
    await completer.future;

    try {
      final vms.Response response = await vmService.callServiceExtension('_flutter.screenshotSkp');
      expect(impellerEnabled, false);
      final String base64data = response.json!['skp'] as String;
      expect(base64data, isNotNull);
      expect(base64data, isNotEmpty);
      final Uint8List decoded = base64Decode(base64data);
      expect(decoded.sublist(0, 8), 'skiapict'.codeUnits);
    } on vms.RPCError catch (e) {
      expect(impellerEnabled, true);
      expect(e.toString(), contains('Cannot capture SKP screenshot with Impeller enabled.'));
    }

    await vmService.dispose();
  });
}
