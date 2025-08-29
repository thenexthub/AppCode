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

import 'dart:ffi';
import 'dart:isolate';
import 'dart:ui';

import 'package:ffi/ffi.dart';
import 'package:test/test.dart';

// This import is used in a test, but not in a way that the analyzer can understand.
// ignore: unused_import
import 'spawn_helper.dart';

@Native<Handle Function(Pointer<Utf8>)>(symbol: 'LoadLibraryFromKernel')
external Object _loadLibraryFromKernel(Pointer<Utf8> path);

@Native<Handle Function(Pointer<Utf8>, Pointer<Utf8>)>(symbol: 'LookupEntryPoint')
external Object _lookupEntryPoint(Pointer<Utf8> library, Pointer<Utf8> name);

@Native<Void Function(Pointer<Utf8>, Pointer<Utf8>)>(symbol: 'Spawn')
external void _spawn(Pointer<Utf8> entrypoint, Pointer<Utf8> route);

void spawn({required SendPort port, String entrypoint = 'main', String route = '/'}) {
  assert(
    entrypoint != 'main' || route != '/',
    'Spawn should not be used to spawn main with the default route name',
  );
  IsolateNameServer.registerPortWithName(port, route);
  final Pointer<Utf8> nativeEntrypoint = entrypoint.toNativeUtf8();
  final Pointer<Utf8> nativeRoute = route.toNativeUtf8();
  _spawn(nativeEntrypoint, nativeRoute);
  malloc.free(nativeEntrypoint);
  malloc.free(nativeRoute);
}

const String kTestEntrypointRouteName = 'testEntrypoint';

@pragma('vm:entry-point')
void testEntrypoint() {
  IsolateNameServer.lookupPortByName(
    kTestEntrypointRouteName,
  )!.send(PlatformDispatcher.instance.defaultRouteName);
}

void main() {
  const bool kReleaseMode = bool.fromEnvironment('dart.vm.product');
  const bool kProfileMode = bool.fromEnvironment('dart.vm.profile');

  test('Spawn a different entrypoint with a special route name', () async {
    final ReceivePort port = ReceivePort();
    spawn(port: port.sendPort, entrypoint: 'testEntrypoint', route: kTestEntrypointRouteName);
    expect(await port.first, kTestEntrypointRouteName);
    port.close();
  });

  test('Lookup entrypoint and execute', () {
    final Pointer<Utf8> libraryPath =
        'file://${const String.fromEnvironment('kFlutterSrcDirectory')}/testing/dart/spawn_helper.dart'
            .toNativeUtf8();
    final Pointer<Utf8> entryPoint = 'echoInt'.toNativeUtf8();
    expect((_lookupEntryPoint(libraryPath, entryPoint) as int Function(int))(42), 42);
    malloc.free(libraryPath);
    malloc.free(entryPoint);
  });

  test('Load from kernel', () {
    final Pointer<Utf8> kernelPath =
        '${const String.fromEnvironment('kFlutterBuildDirectory')}/spawn_helper.dart.dill'
            .toNativeUtf8();
    expect(_loadLibraryFromKernel(kernelPath) is void Function(), true);
    malloc.free(kernelPath);

    final Pointer<Utf8> fakePath = 'fake-path'.toNativeUtf8();
    expect(_loadLibraryFromKernel(fakePath), null);
    malloc.free(fakePath);
  }, skip: kProfileMode || kReleaseMode);
}
