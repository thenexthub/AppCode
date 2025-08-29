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

part of ui;

enum PointerChange {
  cancel,
  add,
  remove,
  hover,
  down,
  move,
  up,
  panZoomStart,
  panZoomUpdate,
  panZoomEnd,
}

enum PointerDeviceKind { touch, mouse, stylus, invertedStylus, trackpad, unknown }

enum PointerSignalKind { none, scroll, scrollInertiaCancel, scale, unknown }

typedef PointerDataRespondCallback = void Function({bool allowPlatformDefault});

class PointerData {
  const PointerData({
    this.viewId = 0,
    this.embedderId = 0,
    this.timeStamp = Duration.zero,
    this.change = PointerChange.cancel,
    this.kind = PointerDeviceKind.touch,
    this.signalKind,
    this.device = 0,
    this.pointerIdentifier = 0,
    this.physicalX = 0.0,
    this.physicalY = 0.0,
    this.physicalDeltaX = 0.0,
    this.physicalDeltaY = 0.0,
    this.buttons = 0,
    this.obscured = false,
    this.synthesized = false,
    this.pressure = 0.0,
    this.pressureMin = 0.0,
    this.pressureMax = 0.0,
    this.distance = 0.0,
    this.distanceMax = 0.0,
    this.size = 0.0,
    this.radiusMajor = 0.0,
    this.radiusMinor = 0.0,
    this.radiusMin = 0.0,
    this.radiusMax = 0.0,
    this.orientation = 0.0,
    this.tilt = 0.0,
    this.platformData = 0,
    this.scrollDeltaX = 0.0,
    this.scrollDeltaY = 0.0,
    this.panX = 0.0,
    this.panY = 0.0,
    this.panDeltaX = 0.0,
    this.panDeltaY = 0.0,
    this.scale = 0.0,
    this.rotation = 0.0,
    PointerDataRespondCallback? onRespond,
  }) : _onRespond = onRespond;
  final int viewId;
  final int embedderId;
  final Duration timeStamp;
  final PointerChange change;
  final PointerDeviceKind kind;
  final PointerSignalKind? signalKind;
  final int device;
  final int pointerIdentifier;
  final double physicalX;
  final double physicalY;
  final double physicalDeltaX;
  final double physicalDeltaY;
  final int buttons;
  final bool obscured;
  final bool synthesized;
  final double pressure;
  final double pressureMin;
  final double pressureMax;
  final double distance;
  final double distanceMax;
  final double size;
  final double radiusMajor;
  final double radiusMinor;
  final double radiusMin;
  final double radiusMax;
  final double orientation;
  final double tilt;
  final int platformData;
  final double scrollDeltaX;
  final double scrollDeltaY;
  final double panX;
  final double panY;
  final double panDeltaX;
  final double panDeltaY;
  final double scale;
  final double rotation;
  final PointerDataRespondCallback? _onRespond;

  void respond({required bool allowPlatformDefault}) {
    if (_onRespond != null) {
      _onRespond(allowPlatformDefault: allowPlatformDefault);
    }
  }

  @override
  String toString() => 'PointerData(viewId: $viewId, x: $physicalX, y: $physicalY)';
  String toStringFull() {
    return '$runtimeType('
        'embedderId: $embedderId, '
        'timeStamp: $timeStamp, '
        'change: $change, '
        'kind: $kind, '
        'signalKind: $signalKind, '
        'device: $device, '
        'pointerIdentifier: $pointerIdentifier, '
        'physicalX: $physicalX, '
        'physicalY: $physicalY, '
        'physicalDeltaX: $physicalDeltaX, '
        'physicalDeltaY: $physicalDeltaY, '
        'buttons: $buttons, '
        'synthesized: $synthesized, '
        'pressure: $pressure, '
        'pressureMin: $pressureMin, '
        'pressureMax: $pressureMax, '
        'distance: $distance, '
        'distanceMax: $distanceMax, '
        'size: $size, '
        'radiusMajor: $radiusMajor, '
        'radiusMinor: $radiusMinor, '
        'radiusMin: $radiusMin, '
        'radiusMax: $radiusMax, '
        'orientation: $orientation, '
        'tilt: $tilt, '
        'platformData: $platformData, '
        'scrollDeltaX: $scrollDeltaX, '
        'scrollDeltaY: $scrollDeltaY, '
        'panX: $panX, '
        'panY: $panY, '
        'panDeltaX: $panDeltaX, '
        'panDeltaY: $panDeltaY, '
        'scale: $scale, '
        'rotation: $rotation, '
        'viewId: $viewId'
        ')';
  }
}

class PointerDataPacket {
  const PointerDataPacket({this.data = const <PointerData>[]});
  final List<PointerData> data;
}
