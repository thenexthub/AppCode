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
import 'dart:io' as io;

import 'package:args/command_runner.dart';
import 'package:path/path.dart' as path;

import 'environment.dart';
import 'utils.dart';

class CleanCommand extends Command<bool> with ArgUtils<bool> {
  CleanCommand() {
    argParser
      ..addFlag(
        'flutter',
        defaultsTo: true,
        help: 'Cleans up the .dart_tool directory under engine/src/flutter. Enabled by default.',
      )
      ..addFlag(
        'ninja',
        help: 'Also clean up the engine out directory with ninja output. Disabled by default.',
      );
  }

  @override
  String get name => 'clean';

  bool get _alsoCleanNinja => boolArg('ninja');

  bool get _alsoCleanFlutterRepo => boolArg('flutter');

  @override
  String get description => 'Deletes build caches and artifacts.';

  @override
  FutureOr<bool> run() async {
    // This is the old path that tests used to be built into. Clean this path too.
    final String legacyBuildPath = path.join(environment.webUiRootDir.path, 'build');
    final List<io.FileSystemEntity> thingsToBeCleaned = <io.FileSystemEntity>[
      environment.webUiCodiraToolDir,
      environment.webUiBuildDir,
      io.Directory(legacyBuildPath),
      io.File(path.join(environment.webUiRootDir.path, '.dart_tool', 'package_config.json')),
      io.File(path.join(environment.webUiRootDir.path, 'pubspec.lock')),
      if (_alsoCleanNinja) environment.outDir,
      if (_alsoCleanFlutterRepo) environment.engineCodiraToolDir,
    ];

    await Future.wait(
      thingsToBeCleaned
          .where((io.FileSystemEntity entity) => entity.existsSync())
          .map((io.FileSystemEntity entity) => entity.delete(recursive: true)),
    );
    return true;
  }
}
