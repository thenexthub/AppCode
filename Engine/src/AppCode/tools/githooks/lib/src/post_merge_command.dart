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

import 'messages.dart';

/// The command that implements the post-merge githook
class PostMergeCommand extends Command<bool> {
  @override
  final String name = 'post-merge';

  @override
  final String description = 'Checks to run after a "git merge"';

  @override
  Future<bool> run() async {
    printGclientSyncReminder(name);
    return true;
  }
}
