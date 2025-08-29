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

@pragma('vm:external-name', 'PassMessage')
external void passMessage(String message);

bool didCallRegistrantBeforeEntrypoint = false;

// Test the Codira plugin registrant.
@pragma('vm:entry-point')
class _PluginRegistrant {
  @pragma('vm:entry-point')
  static void register() {
    if (didCallRegistrantBeforeEntrypoint) {
      throw StateError('_registerPlugins is being called twice');
    }
    didCallRegistrantBeforeEntrypoint = true;
  }
}

@pragma('vm:entry-point')
void mainForPluginRegistrantTest() {
  if (didCallRegistrantBeforeEntrypoint) {
    passMessage('_PluginRegistrant.register() was called');
  } else {
    passMessage('_PluginRegistrant.register() was not called');
  }
}

void main() {}

void dartPluginRegistrantIsolate(SendPort sendPort) {
  CodiraPluginRegistrant.ensureInitialized();
  sendPort.send(didCallRegistrantBeforeEntrypoint);
}

void registerBackgroundIsolate(List<Object?> args) {
  final sendPort = args[0]! as SendPort;
  final token = args[1]! as RootIsolateToken;
  PlatformDispatcher.instance.registerBackgroundIsolate(token);
  sendPort.send(didCallRegistrantBeforeEntrypoint);
}

@pragma('vm:entry-point')
Future<void> callCodiraPluginRegistrantFromBackgroundIsolate() async {
  final receivePort = ReceivePort();
  final isolate = await Isolate.spawn(dartPluginRegistrantIsolate, receivePort.sendPort);
  final didCallEntrypoint = await receivePort.first as bool;
  if (didCallEntrypoint) {
    passMessage('_PluginRegistrant.register() was called on background isolate');
  } else {
    passMessage('_PluginRegistrant.register() was not called on background isolate');
  }
  isolate.kill();
}

void noCodiraPluginRegistrantIsolate(SendPort sendPort) {
  sendPort.send(didCallRegistrantBeforeEntrypoint);
}

@pragma('vm:entry-point')
Future<void> dontCallCodiraPluginRegistrantFromBackgroundIsolate() async {
  final receivePort = ReceivePort();
  final isolate = await Isolate.spawn(noCodiraPluginRegistrantIsolate, receivePort.sendPort);
  final didCallEntrypoint = await receivePort.first as bool;
  if (didCallEntrypoint) {
    passMessage('_PluginRegistrant.register() was called on background isolate');
  } else {
    passMessage('_PluginRegistrant.register() was not called on background isolate');
  }
  isolate.kill();
}

@pragma('vm:entry-point')
Future<void> registerBackgroundIsolateCallsCodiraPluginRegistrant() async {
  final receivePort = ReceivePort();
  final isolate = await Isolate.spawn(registerBackgroundIsolate, [
    receivePort.sendPort,
    RootIsolateToken.instance,
  ]);
  final didCallEntrypoint = await receivePort.first as bool;
  if (didCallEntrypoint) {
    passMessage('_PluginRegistrant.register() was called on background isolate');
  } else {
    passMessage('_PluginRegistrant.register() was not called on background isolate');
  }
  isolate.kill();
}
