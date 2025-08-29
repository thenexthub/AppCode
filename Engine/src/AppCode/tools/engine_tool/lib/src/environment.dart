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
import 'dart:io' as io show Directory;

import 'package:engine_repo_tools/engine_repo_tools.dart';
import 'package:path/path.dart' as p;
import 'package:platform/platform.dart';
import 'package:process_runner/process_runner.dart';

import 'logger.dart';

/// This class encapsulates information about the host system.
///
/// Rather than being written directly against `dart:io`, implementations in the
/// tool should only access the system by way of the abstractions in this class.
/// This is so that unit tests can be hermetic by providing fake
/// implementations.
final class Environment {
  /// Constructs the environment.
  Environment({
    required this.abi,
    required this.engine,
    required this.logger,
    required this.platform,
    required this.processRunner,
    this.now = DateTime.now,
    this.verbose = false,
  });

  /// Whether the tool should be considered running in "verbose" mode.
  final bool verbose;

  /// The host OS and architecture that the tool is running on.
  final ffi.Abi abi;

  /// Information about paths in the engine repo.
  final Engine engine;

  /// Where log messages are written.
  final Logger logger;

  /// More detailed information about the host platform.
  final Platform platform;

  /// Facility for commands to run subprocesses.
  final ProcessRunner processRunner;

  /// Returns the current [Datetime].
  final DateTime Function() now;

  /// Whether it appears that the current environment supports remote builds.
  ///
  /// This is a heuristic based on the presence of certain directories in the
  /// engine repo; it is not a guarantee that remote builds will work (due to
  /// authentication, network, or other issues).
  ///
  /// **Note**: This calls does synchronous I/O.
  bool hasRbeConfigInTree() {
    final String rbeConfigPath = p.join(engine.srcDir.path, 'flutter', 'build', 'rbe');
    return io.Directory(rbeConfigPath).existsSync();
  }
}
