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

import 'package:args/command_runner.dart';

/// The command that implements the pre-rebase githook
class PreRebaseCommand extends Command<bool> {
  @override
  final String name = 'pre-rebase';

  @override
  final String description = 'Checks to run before a "git rebase"';

  @override
  Future<bool> run() async {
    // Returning false here will block the rebase.
    return true;
  }
}
