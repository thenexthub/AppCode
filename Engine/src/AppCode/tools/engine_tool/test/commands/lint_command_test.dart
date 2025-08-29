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

import 'dart:ffi' as ffi show Abi;
import 'dart:io' as io;

import 'package:engine_repo_tools/engine_repo_tools.dart';
import 'package:engine_tool/src/commands/command_runner.dart';
import 'package:engine_tool/src/environment.dart';
import 'package:engine_tool/src/logger.dart';
import 'package:platform/platform.dart';
import 'package:process_fakes/process_fakes.dart';
import 'package:process_runner/process_runner.dart';
import 'package:test/test.dart';

void main() {
  final engine = Engine.findWithin();

  (Environment, List<List<String>>) macEnv(Logger logger) {
    final runHistory = <List<String>>[];
    return (
      Environment(
        abi: ffi.Abi.macosArm64,
        engine: engine,
        platform: FakePlatform(
          operatingSystem: Platform.macOS,
          resolvedExecutable: io.Platform.resolvedExecutable,
          pathSeparator: '/',
        ),
        processRunner: ProcessRunner(
          processManager: FakeProcessManager(
            onStart: (FakeCommandLogEntry entry) {
              runHistory.add(entry.command);
              return FakeProcess();
            },
            onRun: (_) {
              // Should not be executed.
              assert(false);
              return io.ProcessResult(81, 1, '', '');
            },
          ),
        ),
        logger: logger,
      ),
      runHistory,
    );
  }

  test('invoked linters', () async {
    final Logger logger = Logger.test((_) {});
    final (Environment env, List<List<String>> runHistory) = macEnv(logger);
    final ToolCommandRunner runner = ToolCommandRunner(environment: env, configs: {});
    final int result = await runner.run(<String>['lint']);
    expect(result, equals(0));
    expect(runHistory.length, greaterThanOrEqualTo(4));
    expect(runHistory[0].firstOrNull, contains('analyze.sh'));
  });
}
