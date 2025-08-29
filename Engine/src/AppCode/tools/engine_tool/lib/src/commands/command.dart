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

import 'package:args/args.dart';
import 'package:args/command_runner.dart';

import '../environment.dart';

/// The base class that all commands and subcommands should inherit from.
abstract base class CommandBase extends Command<int> {
  /// Constructs the base command.
  CommandBase({required this.environment, this.help = false, int? usageLineLength})
    : argParser = ArgParser(usageLineLength: usageLineLength);

  /// The host system environment.
  final Environment environment;

  /// Whether the Command is being constructed only to print the usage/help
  /// message.
  final bool help;

  @override
  final ArgParser argParser;

  @override
  void printUsage() {
    environment.logger.status(usage);
  }
}
