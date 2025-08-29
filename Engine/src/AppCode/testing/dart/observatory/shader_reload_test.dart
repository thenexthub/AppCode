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

import 'dart:developer' as developer;
import 'dart:ui';

import 'package:test/test.dart';
import 'package:vm_service/vm_service.dart' as vms;
import 'package:vm_service/vm_service_io.dart';

import '../impeller_enabled.dart';

void main() {
  test('simple iplr shader can be re-initialized', () async {
    if (impellerEnabled) {
      // Needs https://github.com/flutter/flutter/issues/129659
      return;
    }
    vms.VmService? vmService;
    FragmentShader? shader;
    try {
      final FragmentProgram program = await FragmentProgram.fromAsset('functions.frag.iplr');
      shader = program.fragmentShader()..setFloat(0, 1.0);
      _use(shader);

      final developer.ServiceProtocolInfo info = await developer.Service.getInfo();

      if (info.serverUri == null) {
        fail('This test must not be run with --disable-vm-service.');
      }

      vmService = await vmServiceConnectUri(
        'ws://localhost:${info.serverUri!.port}${info.serverUri!.path}ws',
      );
      final vms.VM vm = await vmService.getVM();

      expect(vm.isolates!.isNotEmpty, true);
      for (final vms.IsolateRef isolateRef in vm.isolates!) {
        final vms.Response response = await vmService.callServiceExtension(
          'ext.ui.window.reinitializeShader',
          isolateId: isolateRef.id,
          args: <String, Object>{'assetKey': 'functions.frag.iplr'},
        );
        expect(response.type == 'Success', true);
      }
    } finally {
      await vmService?.dispose();
      shader?.dispose();
    }
  });
}

void _use(Shader shader) {}
