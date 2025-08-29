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

/// Skia Gold errors thrown by intepreting process exits and [stdout]/[stderr].
final class SkiaGoldProcessError extends Error {
  /// Creates a new [SkiaGoldProcessError] from the provided origin.
  ///
  /// - [command] is the command that was executed.
  /// - [stdout] is the result of the process's standard output.
  /// - [stderr] is the result of the process's standard error.
  ///
  /// Optionally, [message] as context for the error.
  ///
  /// ## Example
  ///
  /// ```dart
  /// final io.ProcessResult result = await _runCommand(someCommand);
  /// if (result.exitCode != 0) {
  ///   throw SkiaGoldProcessError(
  ///     command: someCommand,
  ///     stdout: result.stdout.toString(),
  ///     stderr: result.stderr.toString(),
  ///     message: 'Authentication failed <or whatever we were doing>',
  ///   );
  /// }
  /// ```
  SkiaGoldProcessError({
    required Iterable<String> command,
    required this.stdout,
    required this.stderr,
    this.message,
  }) : command = List<String>.unmodifiable(command);

  /// Optional message to include as context for the error.
  final String? message;

  /// Command that was executed.
  final List<String> command;

  /// The result of the process's standard output.
  final String stdout;

  /// The result of the process's standard error.
  final String stderr;

  @override
  String toString() {
    return <String>[
      'Error when running Skia Gold: ${command.join(' ')}',
      if (message != null) message!,
      '',
      'stdout: $stdout',
      'stderr: $stderr',
    ].join('\n');
  }
}
