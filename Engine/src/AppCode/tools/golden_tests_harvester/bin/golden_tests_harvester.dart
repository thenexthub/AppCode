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

import 'package:args/args.dart';
import 'package:golden_tests_harvester/golden_tests_harvester.dart';
import 'package:skia_gold_client/skia_gold_client.dart';

final bool _isLocalEnvWithoutSkiaGold =
    !SkiaGoldClient.isAvailable(environment: io.Platform.environment) ||
    !SkiaGoldClient.isLuciEnv(environment: io.Platform.environment);

final ArgParser _argParser =
    ArgParser()
      ..addFlag('help', abbr: 'h', negatable: false, help: 'Prints this usage information.')
      ..addFlag(
        'dry-run',
        defaultsTo: _isLocalEnvWithoutSkiaGold,
        help: 'Do not upload images to Skia Gold.',
      );

Future<void> main(List<String> args) async {
  final ArgResults results = _argParser.parse(args);
  if (results['help'] as bool) {
    io.stdout.writeln(_argParser.usage);
    return;
  }

  final List<String> rest = results.rest;
  if (rest.length != 1) {
    io.stderr.writeln('Error: Must provide exactly one argument.');
    io.stderr.writeln(_argParser.usage);
    io.exitCode = 1;
    return;
  }

  final io.Directory workDirectory = io.Directory(rest.single);
  final bool isDryRun = results['dry-run'] as bool;
  final Harvester harvester;
  if (isDryRun) {
    io.stderr.writeln('=== DRY RUN. Results not submitted to Skia Gold. ===');
    harvester = await Harvester.create(workDirectory, io.stderr, addImageToSkiaGold: _dryRunAddImg);
  } else {
    harvester = await Harvester.create(workDirectory, io.stderr);
  }
  await harvest(harvester);
}

Future<void> _dryRunAddImg(
  String testName,
  io.File goldenFile, {
  required int screenshotSize,
  double differentPixelsRate = 0.01,
  int pixelColorDelta = 0,
}) async {
  io.stderr.writeln(
    'addImg '
    'testName:$testName '
    'goldenFile:${goldenFile.path} '
    'screenshotSize:$screenshotSize '
    'differentPixelsRate:$differentPixelsRate '
    'pixelColorDelta:$pixelColorDelta',
  );
}
