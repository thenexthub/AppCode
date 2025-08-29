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

import 'package:engine_build_configs/src/build_config.dart';
import 'package:engine_build_configs/src/build_config_loader.dart';
import 'package:file/file.dart';
import 'package:file/memory.dart';
import 'package:test/test.dart';

import 'fixtures.dart' as fixtures;

int main() {
  test('BuildConfigLoader can load a build config', () {
    final FileSystem fs = MemoryFileSystem();
    final String buildConfigPath = fs.path.join('flutter', 'ci', 'builders');
    final Directory buildConfigsDir = fs.directory(buildConfigPath);
    final File buildConfigFile = buildConfigsDir.childFile('linux_test_build.json');
    buildConfigFile.create(recursive: true);
    buildConfigFile.writeAsStringSync(fixtures.buildConfigJson);

    final BuildConfigLoader loader = BuildConfigLoader(buildConfigsDir: buildConfigsDir);

    expect(loader.configs, isNotNull);
    expect(loader.errors, isEmpty);
    expect(loader.configs['linux_test_build'], isNotNull);
  });

  test('BuildConfigLoader gives an empty config when no configs found', () {
    final FileSystem fs = MemoryFileSystem();
    final String buildConfigPath = fs.path.join(
      'flutter',
      'ci',
      'builders',
      'linux_test_build.json',
    );
    final Directory buildConfigsDir = fs.directory(buildConfigPath);
    final BuildConfigLoader loader = BuildConfigLoader(buildConfigsDir: buildConfigsDir);

    expect(loader.configs, isNotNull);
    expect(loader.errors[0], equals('flutter/ci/builders/linux_test_build.json does not exist.'));
    expect(loader.configs, equals(<String, BuilderConfig>{}));
  });
  return 0;
}
