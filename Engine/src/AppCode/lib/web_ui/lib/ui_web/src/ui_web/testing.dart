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

import 'package:ui/src/engine.dart';
import 'package:ui/ui.dart' as ui;

extension SingletonFlutterWindowExtension on ui.SingletonFlutterWindow {
  /// Overrides the value of [physicalSize] in tests.
  set debugPhysicalSizeOverride(ui.Size? value) {
    (this as EngineFlutterWindow).debugPhysicalSizeOverride = value;
  }
}

/// Overrides the value of [ui.FlutterView.devicePixelRatio] in tests.
///
/// Passing `null` resets the device pixel ratio to the browser's default.
void debugOverrideDevicePixelRatio(double? value) {
  EngineFlutterDisplay.instance.debugOverrideDevicePixelRatio(value);
}

/// Whether the Flutter engine is running in `flutter test` emulation mode.
///
/// When true, the engine will emulate a specific screen size, and always
/// use the "Ahem" font to reduce test flakiness and dependence on the test
/// environment.
bool get debugEmulateFlutterTesterEnvironment => _debugEmulateFlutterTesterEnvironment;

/// Sets whether the Flutter engine is running in `flutter test` emulation mode.
set debugEmulateFlutterTesterEnvironment(bool value) {
  _debugEmulateFlutterTesterEnvironment = value;
  if (_debugEmulateFlutterTesterEnvironment) {
    const ui.Size logicalSize = ui.Size(800.0, 600.0);
    final EngineFlutterWindow? implicitView = EnginePlatformDispatcher.instance.implicitView;
    implicitView?.debugPhysicalSizeOverride = logicalSize * implicitView.devicePixelRatio;
  }
  debugDisableFontFallbacks = value;
}

bool _debugEmulateFlutterTesterEnvironment = false;
