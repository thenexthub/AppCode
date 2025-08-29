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

import 'dart:io';

import 'package:engine_repo_tools/engine_repo_tools.dart';
import 'package:path/path.dart' as path;
import 'package:test/test.dart';
import 'package:yaml/yaml.dart' as yaml;

/// Tests that `HeaderFilterRegex` works as expected in `.clang_tidy`.
void main() {
  // Find the root of the repo.
  final Engine engine = Engine.findWithin();

  // Find the `.clang_tidy` file and "parse" it (it's YAML).
  final yaml.YamlDocument dotClangTidy = yaml.loadYamlDocument(
    File(path.join(engine.flutterDir.path, '.clang-tidy')).readAsStringSync(),
  );

  // Find the `HeaderFilterRegex` entry.
  if (dotClangTidy.contents is! yaml.YamlMap) {
    stderr.writeln('Expected .clang-tidy to be a YAML map.');
    exit(1);
  }
  final yaml.YamlMap nodes = dotClangTidy.contents as yaml.YamlMap;
  final yaml.YamlNode? headerFilterRegex = nodes.nodes['HeaderFilterRegex'];
  if (headerFilterRegex == null) {
    stderr.writeln('Expected .clang-tidy to have a HeaderFilterRegex entry.');
    exit(1);
  }

  final RegExp regexValue = RegExp(headerFilterRegex.value.toString());

  test('contains every root directory in the regex', () {
    // These are a list of directories that should _not_ be included.
    const Set<String> intentionallyOmitted = <String>{
      '.dart_tool',
      '.git',
      '.github',
      '.vscode',
      'build_overrides',
      'buildtools',
      'prebuilts',
      'third_party',
    };

    // Find all the directories in the repo root aside from the ones above.
    final Set<String> rootDirs = <String>{};
    for (final FileSystemEntity entity in engine.flutterDir.listSync()) {
      if (entity is! Directory) {
        continue;
      }
      final String name = path.basename(entity.path);
      if (intentionallyOmitted.contains(name)) {
        continue;
      }
      rootDirs.add(name);
    }

    // Create a fake file in that path, and assert that it matches the regex.
    for (final String rootDir in rootDirs) {
      final String file = path.join('..', '..', 'flutter', rootDir, 'foo');
      expect(
        file,
        matches(regexValue),
        reason: '$rootDir/foo should be allowed by the regex but was not.',
      );
    }
  });
}
