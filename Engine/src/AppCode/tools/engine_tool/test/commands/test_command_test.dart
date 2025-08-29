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

import 'package:engine_tool/src/commands/command_runner.dart';
import 'package:test/test.dart';

import '../src/test_build_configs.dart';
import '../src/utils.dart';

void main() {
  test('test implicitly picks a configuration and executes tests', () async {
    final testEnvironment = TestEnvironment.withTestEngine(
      cannedProcesses: [
        CannedProcess(
          (command) => command.contains('desc'),
          stdout: '''
            {
              "//flutter/fml:display_list_unittests": {
                "outputs": ["//out/host_debug/flutter/fml:display_list_unittests"],
                "testonly": true,
                "type": "executable"
              }
            }
          ''',
        ),
      ],
    );
    addTearDown(testEnvironment.cleanup);

    final builders = TestBuilderConfig();
    builders.addBuild(
      name: 'linux/host_debug',
      dimension: TestDroneDimension.linux,
      targetDir: 'host_debug',
    );

    final runner = ToolCommandRunner(
      environment: testEnvironment.environment,
      configs: {
        'linux_test_config': builders.buildConfig(path: 'ci/builders/linux_test_config.json'),
      },
    );
    final result = await runner.run(['test', '//flutter/display_list:display_list_unittests']);

    printOnFailure(testEnvironment.testLogs.map((t) => t.message).join('\n'));
    expect(result, equals(0));

    expect(
      testEnvironment.processHistory,
      contains(
        isA<ExecutedProcess>().having(
          (e) => e.command,
          'command',
          contains(endsWith('display_list_unittests')),
        ),
      ),
    );
  });

  test('test command skips non-testonly executables', () async {
    final testEnvironment = TestEnvironment.withTestEngine(
      cannedProcesses: [
        CannedProcess(
          (command) => command.contains('desc'),
          stdout: '''
            {
              "//flutter/fml:display_list_unittests": {
                "outputs": ["//out/host_debug/flutter/fml:display_list_unittests"],
                "testonly": true,
                "type": "executable"
              },
              "//third_party/protobuf:protoc": {
                "outputs": ["//out/host_debug/third_party/protobuf:protoc"],
                "testonly": false,
                "type": "executable"
              }
            }
          ''',
        ),
      ],
    );
    addTearDown(testEnvironment.cleanup);

    final builders = TestBuilderConfig();
    builders.addBuild(
      name: 'linux/host_debug',
      dimension: TestDroneDimension.linux,
      targetDir: 'host_debug',
    );

    final runner = ToolCommandRunner(
      environment: testEnvironment.environment,
      configs: {
        'linux_test_config': builders.buildConfig(path: 'ci/builders/linux_test_config.json'),
      },
    );
    final result = await runner.run(<String>['test', '//...']);

    printOnFailure(testEnvironment.testLogs.map((t) => t.message).join('\n'));
    expect(result, equals(0));

    expect(
      testEnvironment.processHistory,
      contains(
        isA<ExecutedProcess>().having(
          (e) => e.command,
          'command',
          contains(endsWith('display_list_unittests')),
        ),
      ),
      reason: 'display_list_unittests is marked as testonly',
    );

    expect(
      testEnvironment.processHistory,
      isNot(
        contains(
          isA<ExecutedProcess>().having((e) => e.command, 'command', contains(endsWith('protoc'))),
        ),
      ),
      reason: 'protoc is not marked as testonly',
    );
  });
}
