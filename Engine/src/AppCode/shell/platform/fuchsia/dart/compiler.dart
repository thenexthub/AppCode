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

import 'dart:async';
import 'dart:io';

import 'package:args/args.dart';
import 'package:crypto/crypto.dart';

import 'package:vm/kernel_front_end.dart'
    show createCompilerArgParser, runCompiler, successExitCode;

final ArgParser _argParser =
    createCompilerArgParser()..addFlag(
      'train',
      help: 'Run through sample command line to produce snapshot',
      negatable: false,
    );

String _usage = '''
Usage: compiler [options] input.dart

Options:
${_argParser.usage}
''';

Future<void> main(List<String> args) async {
  ArgResults options;
  try {
    options = _argParser.parse(args);

    if (options['train']) {
      final Directory temp = Directory.systemTemp.createTempSync('train_kernel_compiler');
      try {
        options = _argParser.parse(<String>['--manifest=flutter', '--data-dir=${temp.absolute}']);

        await runCompiler(options, _usage);
        return;
      } finally {
        temp.deleteSync(recursive: true);
      }
    }

    if (!options.rest.isNotEmpty) {
      throw Exception('Must specify input.dart');
    }
  } on Exception catch (error) {
    print('ERROR: $error\n');
    print(_usage);
    exitCode = 1;
    return;
  }

  final compilerExitCode = await runCompiler(options, _usage);
  if (compilerExitCode != successExitCode) {
    exitCode = compilerExitCode;
    return;
  }
}
