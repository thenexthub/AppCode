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

import 'package:process_runner/process_runner.dart';

import 'dart_utils.dart';
import 'environment.dart';
import 'logger.dart';

/// Update Flutter engine dependencies. Returns an exit code.
Future<int> fetchDependencies(Environment environment) async {
  if (!environment.processRunner.processManager.canRun('gclient')) {
    environment.logger.error('Cannot find the gclient command in your path');
    return 1;
  }

  final dotGclientPath = findDotGclient(environment);

  if (dotGclientPath == null) {
    environment.logger.error(
      'Failed to find the .gclient file. Make sure your local engine build '
      'environment is configured as described in '
      'https://github.com/flutter/flutter/blob/master/engine/README.md',
    );
    return 1;
  }

  environment.logger.status('Fetching dependencies... ', newline: environment.verbose);

  final dotGclient = io.File(dotGclientPath);

  Spinner? spinner;
  ProcessRunnerResult result;
  try {
    if (!environment.verbose) {
      spinner = environment.logger.startSpinner();
    }

    result = await environment.processRunner.runProcess(
      <String>['gclient', 'sync', '-D'],
      runInShell: true,
      startMode:
          environment.verbose ? io.ProcessStartMode.inheritStdio : io.ProcessStartMode.normal,
      workingDirectory: dotGclient.parent,
    );
  } finally {
    spinner?.finish();
  }

  if (result.exitCode != 0) {
    environment.logger.error('Fetching dependencies failed.');

    // Verbose mode already logged output by making the child process inherit
    // this process's stdio handles.
    if (!environment.verbose) {
      environment.logger.error('Output:\n${result.output}');
    }
  }

  return result.exitCode;
}
