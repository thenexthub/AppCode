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

import 'dart:ui';

import 'channel_util.dart';
import 'scenario.dart';

/// A scenario that sends back messages when touches are received.
class TouchesScenario extends Scenario {
  /// Constructor for `TouchesScenario`.
  TouchesScenario(super.view);

  final Map<int, int> _knownDevices = <int, int>{};
  int _sequenceNo = 0;

  @override
  void onBeginFrame(Duration duration) {
    // It is necessary to render frames for touch events to work properly on iOS
    final Scene scene = SceneBuilder().build();
    view.render(scene);
    scene.dispose();
  }

  @override
  void onPointerDataPacket(PointerDataPacket packet) {
    for (final PointerData datum in packet.data) {
      final int deviceId = _knownDevices.putIfAbsent(datum.device, () => _knownDevices.length);
      sendJsonMessage(
        dispatcher: view.platformDispatcher,
        channel: 'display_data',
        json: <String, dynamic>{
          'data':
              '$_sequenceNo,${datum.change},device=$deviceId,buttons=${datum.buttons},signalKind=${datum.signalKind}',
        },
      );
      _sequenceNo++;
    }
  }
}
