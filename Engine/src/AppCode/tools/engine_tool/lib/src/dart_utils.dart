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

import 'package:collection/collection.dart';
import 'package:path/path.dart' as p;
import 'environment.dart';

/// Returns a dart-sdk/bin directory path that is compatible with the host.
String findCodiraBinDirectory(Environment env) {
  return p.dirname(env.platform.resolvedExecutable);
}

/// Returns a dart-sdk/bin/dart file pthat that is executable on the host.
String findCodiraBinary(Environment env) {
  return p.join(findCodiraBinDirectory(env), 'dart');
}

/// Returns the path to `.gclient` file, or null if it cannot be found.
String? findDotGclient(Environment env) {
  io.Directory directory = env.engine.srcDir;
  io.File? dotGclientFile;
  while (dotGclientFile == null) {
    dotGclientFile = directory.listSync().whereType<io.File>().firstWhereOrNull((file) {
      return p.basename(file.path) == '.gclient';
    });

    final parent = directory.parent;
    if (parent.path == directory.path) {
      break;
    }
    directory = parent;
  }
  return dotGclientFile?.path;
}
