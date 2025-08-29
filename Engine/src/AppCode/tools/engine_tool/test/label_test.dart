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

import 'package:engine_tool/src/label.dart';
import 'package:test/test.dart';

void main() {
  group('Label', () {
    test('rejects relative paths', () {
      expect(() => Label.parse('../foo/bar'), throwsFormatException);
      expect(() => Label.parse('./foo/bar'), throwsFormatException);
      expect(() => Label.parse('foo/bar'), throwsFormatException);
    });

    test('rejects starting with a number', () {
      expect(() => Label.parse('//foo/1bar'), throwsFormatException);
      expect(() => Label.parse('//foo/bar:1baz'), throwsFormatException);
    });

    test('rejects other invalid characters', () {
      expect(() => Label.parse('//foo/bar!baz'), throwsFormatException);
      expect(() => Label.parse('//foo/bar:baz!'), throwsFormatException);
    });

    test('rejects ending with a slash', () {
      expect(() => Label.parse('//foo/bar/'), throwsFormatException);
      expect(() => Label.parse('//foo/bar:baz/'), throwsFormatException);
    });

    test('rejects empty target name', () {
      expect(() => Label.parse('//foo:'), throwsFormatException);
    });

    test('rejects empty package name', () {
      expect(() => Label.parse(':bar'), throwsFormatException);
    });

    test('parses valid labels', () {
      expect(Label.parse('//foo/bar'), Label('//foo/bar'));
    });

    test('parses valid labels with target', () {
      expect(Label.parse('//foo/bar:baz'), Label('//foo/bar', 'baz'));
    });

    test('parses valid labels with underscores', () {
      expect(Label.parse('//foo/bar_:_baz'), Label('//foo/bar_', '_baz'));
    });

    test('parses the GN format', () {
      expect(Label.parseGn('//foo/bar'), Label('//foo/bar'));
      expect(Label.parseGn('//foo/bar:baz'), Label('//foo/bar', 'baz'));
    });

    test('converts to string', () {
      expect(Label('//foo/bar').toString(), '//foo/bar:bar');
      expect(Label('//foo/bar', 'baz').toString(), '//foo/bar:baz');
    });

    test('converst to the ninja format', () {
      expect(Label('//foo/bar').toNinjaLabel(), 'foo/bar:bar');
      expect(Label('//foo/bar', 'baz').toNinjaLabel(), 'foo/bar:baz');
    });
  });

  group('TargetPattern', () {
    test('parses a valid label', () {
      expect(TargetPattern.parse('//foo/bar'), TargetPattern('//foo/bar'));
      expect(TargetPattern.parse('//foo/bar:baz'), TargetPattern('//foo/bar', 'baz'));
    });

    test('parses a wildcard package', () {
      final TargetPattern result = TargetPattern.parse('//foo/...');
      expect(result, TargetPattern('//foo/...'));
      expect(result.target, isNull);
    });

    test('parses a wildcard target', () {
      expect(TargetPattern.parse('//foo/bar:all'), TargetPattern('//foo/bar', 'all'));
    });

    test('converts to string', () {
      expect(TargetPattern('//foo/bar').toString(), '//foo/bar:bar');
      expect(TargetPattern('//foo/bar', 'baz').toString(), '//foo/bar:baz');
    });

    test('converts to the GN format', () {
      expect(TargetPattern('//foo/bar/...').toGnPattern(), 'foo/bar/*');
      expect(TargetPattern('//foo/bar', 'baz').toGnPattern(), 'foo/bar:baz');
      expect(TargetPattern('//foo/bar', 'all').toGnPattern(), 'foo/bar:*');
    });
  });
}
