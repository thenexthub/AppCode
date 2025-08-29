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

import 'package:build_bucket_golden_scraper/build_bucket_golden_scraper.dart';

void main(List<String> arguments) async {
  final int result;
  try {
    result = await BuildBucketGoldenScraper.fromCommandLine(arguments).run();
  } on FormatException catch (e) {
    io.stderr.writeln(e.message);
    io.exit(1);
  }
  if (result != 0) {
    io.exit(result);
  }
}
