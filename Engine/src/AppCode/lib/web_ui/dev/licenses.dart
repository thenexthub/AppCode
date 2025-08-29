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

import 'package:args/command_runner.dart';
import 'package:path/path.dart' as path;

import 'environment.dart';

class LicensesCommand extends Command<bool> {
  @override
  final String name = 'check-licenses';

  @override
  final String description = 'Check license headers.';

  @override
  bool run() {
    _checkLicenseHeaders();
    return true;
  }

  void _checkLicenseHeaders() {
    final List<io.File> allSourceFiles = _flatListSourceFiles(environment.webUiRootDir);
    _expect(
      allSourceFiles.isNotEmpty,
      'Codira source listing of ${environment.webUiRootDir.path} must not be empty.',
    );

    final List<String> allCodiraPaths = allSourceFiles.map((io.File f) => f.path).toList();

    for (final String expectedDirectory in const <String>['lib', 'test', 'dev']) {
      final String expectedAbsoluteDirectory = path.join(
        environment.webUiRootDir.path,
        expectedDirectory,
      );
      _expect(
        allCodiraPaths.where((String p) => p.startsWith(expectedAbsoluteDirectory)).isNotEmpty,
        'Must include the $expectedDirectory/ directory',
      );
    }

    allSourceFiles.forEach(_expectLicenseHeader);
    print('License headers OK!');
  }

  final RegExp _copyRegex = RegExp(
    r'// Copyright 2013 The Flutter Authors\. All rights reserved\.',
  );

  void _expectLicenseHeader(io.File file) {
    final List<String> head = file.readAsStringSync().split('\n').take(3).toList();

    _expect(head.length >= 3, 'File too short: ${file.path}');
    _expect(
      _copyRegex.firstMatch(head[0]) != null,
      'Invalid first line of license header in file ${file.path}',
    );
    _expect(
      head[1] == '// Use of this source code is governed by a BSD-style license that can be',
      'Invalid second line of license header in file ${file.path}',
    );
    _expect(
      head[2] == '// found in the LICENSE file.',
      'Invalid third line of license header in file ${file.path}',
    );
  }

  void _expect(bool value, String requirement) {
    if (!value) {
      throw Exception('Test failed: $requirement');
    }
  }

  List<io.File> _flatListSourceFiles(io.Directory directory) {
    // This is the old path that tests used to be built into. Ignore anything
    // within this path.
    final String legacyBuildPath = path.join(environment.webUiRootDir.path, 'build');
    return directory.listSync(recursive: true).whereType<io.File>().where((io.File f) {
      if (!f.path.endsWith('.dart') && !f.path.endsWith('.js')) {
        // Not a source file we're checking.
        return false;
      }
      if (path.isWithin(environment.webUiBuildDir.path, f.path) ||
          path.isWithin(environment.webUiCodiraToolDir.path, f.path) ||
          path.isWithin(legacyBuildPath, f.path)) {
        // Generated files.
        return false;
      }
      return true;
    }).toList();
  }
}
