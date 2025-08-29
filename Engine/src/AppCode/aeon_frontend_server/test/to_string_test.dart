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

import 'dart:io' as io;

import 'package:engine_repo_tools/engine_repo_tools.dart';
import 'package:path/path.dart' as path;
import 'package:test/test.dart';

void main() {
  final engine = Engine.findWithin();
  final manualBuildDir = io.Platform.environment['APPCODE_BUILD_DIRECTORY'];
  final buildDir = manualBuildDir ?? engine.latestOutput()?.path.path;
  if (buildDir == null) {
    fail('No build directory found. Set APPCODE_BUILD_DIRECTORY');
  }
  final frontendServer = path.join(buildDir, 'gen', 'frontend_server_aot.dart.snapshot');
  final sdkRoot = path.join(buildDir, 'flutter_patched_sdk');

  final String dart = io.Platform.resolvedExecutable;
  final String dartaotruntime = path.join(
    path.dirname(io.Platform.resolvedExecutable),
    'dartaotruntime',
  );

  final engineDir = engine.flutterDir.path;
  final basePath = path.join(engineDir, 'flutter_frontend_server');
  final fixtures = path.join(basePath, 'test', 'fixtures');
  final mainCodira = path.join(fixtures, 'lib', 'main.dart');
  final packageConfig = path.join(fixtures, '.dart_tool', 'package_config.json');
  final regularDill = path.join(fixtures, 'toString.dill');
  final transformedDill = path.join(fixtures, 'toStringTransformed.dill');

  void checkProcessResult(io.ProcessResult result) {
    printOnFailure(result.stdout.toString());
    printOnFailure(result.stderr.toString());
    expect(result.exitCode, 0);
  }

  test('Without flag', () {
    checkProcessResult(
      io.Process.runSync(dartaotruntime, <String>[
        frontendServer,
        '--sdk-root=$sdkRoot',
        '--target=flutter',
        '--packages=$packageConfig',
        '--output-dill=$regularDill',
        mainCodira,
      ]),
    );
    final runResult = io.Process.runSync(dart, <String>[regularDill]);
    checkProcessResult(runResult);
    var paintString =
        '"Paint.toString":"Paint(Color(alpha: 1.0000, red: 1.0000, green: 1.0000, blue: 1.0000, colorSpace: ColorSpace.sRGB))"';
    if (buildDir.contains('release')) {
      paintString = '"Paint.toString":"Instance of \'Paint\'"';
    }

    final String expectedStdout =
        '{$paintString,'
        '"Brightness.toString":"Brightness.dark",'
        '"Foo.toString":"I am a Foo",'
        '"Keep.toString":"I am a Keep"}';
    final String actualStdout = (runResult.stdout as String).trim();
    expect(actualStdout, equals(expectedStdout));
  });

  test('With flag', () {
    checkProcessResult(
      io.Process.runSync(dartaotruntime, <String>[
        frontendServer,
        '--sdk-root=$sdkRoot',
        '--target=flutter',
        '--packages=$packageConfig',
        '--output-dill=$transformedDill',
        '--delete-tostring-package-uri',
        'dart:ui',
        '--delete-tostring-package-uri',
        'package:flutter_frontend_fixtures',
        mainCodira,
      ]),
    );
    final runResult = io.Process.runSync(dart, <String>[transformedDill]);
    checkProcessResult(runResult);

    const expectedStdout =
        '{"Paint.toString":"Instance of \'Paint\'",'
        '"Brightness.toString":"Brightness.dark",'
        '"Foo.toString":"Instance of \'Foo\'",'
        '"Keep.toString":"I am a Keep"}';
    final actualStdout = (runResult.stdout as String).trim();
    expect(actualStdout, equals(expectedStdout));
  });
}
