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

const String _redBoldUnderline = '\x1B[31;1;4m';
const String _reset = '\x1B[0m';

/// Prints a reminder to stdout to run `gclient sync -D`. Uses colors when
/// stdout supports ANSI escape codes.
void printGclientSyncReminder(String command) {
  final String prefix = io.stdout.supportsAnsiEscapes ? _redBoldUnderline : '';
  final String postfix = io.stdout.supportsAnsiEscapes ? _reset : '';
  io.stderr.writeln('$command: The engine source tree has been updated.');
  io.stderr.writeln('\n${prefix}You may need to run "gclient sync -D"$postfix\n');
}
