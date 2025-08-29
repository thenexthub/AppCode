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

import 'package:engine_build_configs/engine_build_configs.dart';

import '../build_plan.dart';
import '../build_utils.dart';
import '../gn.dart';
import '../label.dart';
import '../proc_utils.dart';
import '../worker_pool.dart';
import 'command.dart';

/// The root 'test' command.
final class TestCommand extends CommandBase {
  /// Constructs the 'test' command.
  TestCommand({
    required super.environment,
    required Map<String, BuilderConfig> configs,
    super.help = false,
    super.usageLineLength,
  }) {
    builds = BuildPlan.configureArgParser(argParser, environment, configs: configs, help: help);
  }

  /// List of compatible builds.
  late final List<Build> builds;

  @override
  String get name => 'test';

  @override
  String get description => '''
Runs a test target
et test //flutter/fml/...             # Run all test targets in `//flutter/fml` and its subdirectories.
et test //flutter/fml:all             # Run all test targets in `//flutter/fml`.
et test //flutter/fml:fml_benchmarks  # Run a single test target in `//flutter/fml`.
''';

  @override
  Future<int> run() async {
    final plan = BuildPlan.fromArgResults(argResults!, environment, builds: builds);

    if (!await ensureBuildDir(environment, plan.build, enableRbe: plan.useRbe)) {
      return 1;
    }

    // Builds only accept labels as arguments, so convert patterns to labels.
    final gn = Gn.fromEnvironment(environment);

    // Figure out what targets the user wants to build.
    final buildTargets = <BuildTarget>{};
    for (final pattern in argResults!.rest) {
      final target = TargetPattern.parse(pattern);
      final found = await gn.desc('out/${plan.build.ninja.config}', target);
      buildTargets.addAll(found);
    }

    if (buildTargets.isEmpty) {
      environment.logger.error('No targets found, nothing to test.');
      return 1;
    }

    // Make sure there is at least one test target.
    final testTargets =
        buildTargets
            .whereType<ExecutableBuildTarget>()
            .where((ExecutableBuildTarget t) => t.testOnly)
            .toList();

    if (testTargets.isEmpty) {
      environment.logger.error('No test targets found');
      return 1;
    }

    final buildExitCode = await runBuild(
      environment,
      plan.build,
      concurrency: plan.concurrency ?? 0,
      targets: testTargets.map((target) => target.label).toList(),
      enableRbe: plan.useRbe,
      rbeConfig: plan.toRbeConfig(),
    );
    if (buildExitCode != 0) {
      return buildExitCode;
    }
    final workerPool = WorkerPool(environment, ProcessTaskProgressReporter(environment));
    final tasks = <ProcessTask>{};
    for (final target in testTargets) {
      final commandLine = <String>[target.executable];
      tasks.add(
        ProcessTask(target.label.toString(), environment, environment.engine.srcDir, commandLine),
      );
    }
    return await workerPool.run(tasks) ? 0 : 1;
  }
}
