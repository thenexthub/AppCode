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

import 'package:engine_build_configs/src/merge_gn_args.dart';
import 'package:test/test.dart';

void main() {
  test('refuses to merge with arguments that do not start with --', () {
    expect(() => mergeGnArgs(buildArgs: [], extraArgs: ['foo']), throwsArgumentError);
  });

  test('refuses to merge with arguments that contain spaces', () {
    expect(() => mergeGnArgs(buildArgs: [], extraArgs: ['--foo bar']), throwsArgumentError);
  });

  test('refuses to merge with arguments that contain equals', () {
    expect(() => mergeGnArgs(buildArgs: [], extraArgs: ['--foo=bar']), throwsArgumentError);
  });

  test('appends if there are no matching arguments', () {
    expect(mergeGnArgs(buildArgs: ['--foo'], extraArgs: ['--bar']), ['--foo', '--bar']);
  });

  test('replaces --foo with --no-foo', () {
    expect(mergeGnArgs(buildArgs: ['--foo'], extraArgs: ['--no-foo']), ['--no-foo']);
  });

  test('replaces --no-foo with --foo', () {
    expect(mergeGnArgs(buildArgs: ['--no-foo'], extraArgs: ['--foo']), ['--foo']);
  });
}
