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

import 'package:licenses/formatter.dart';
import 'package:test/test.dart';

void main() {
  test('Block comments', () {
    expect(reformat('/* test */'), 'test');
    expect(reformat('/*\ntest\n*/'), 'test');
    expect(reformat('/*\ntest\n */'), 'test');
    expect(reformat('/*\n * test\n*/'), 'test');
    expect(reformat('/*\n * test\n */'), 'test');
    expect(reformat('/*\n * test\n * test\n */'), 'test\ntest');
    expect(reformat('/*\n * test\n + test\n */'), '* test\n+ test');
    expect(reformat('test */'), 'test');
  });
  test('Indenting blocks', () {
    expect(reformat('  a\nb\n  c'), 'a\nb\n  c'); // strips leading indents
    expect(
      reformat('  a\n b\n  c'),
      'a\nb\nc',
    ); // strips common one-space indent, then strips stray one-space indents
    expect(reformat('  a\n  b\n  c'), 'a\nb\nc'); // strips common two-space indent
    expect(
      reformat('  a\n   b\n  c'),
      'a\nb\nc',
    ); // strips common two-space indent, then strips stray one-space indent
    expect(reformat('  a\n    b\n  c'), 'a\n  b\nc'); // streps common two-space indent
    expect(reformat('  a\n     b\n  c'), 'a\n   b\nc'); // streps common two-space indent
  });
  test('Leading blocks', () {
    expect(reformat('#; a\n#; b\n#; c'), 'a\nb\nc');
    expect(reformat('#; a\nb\nc'), '#; a\nb\nc');
    expect(reformat('#; a\n  b\n  c'), '#; a\n  b\n  c');
  });
  test('Leading indented lines', () {
    expect(reformat('  a\nb\nc'), 'a\nb\nc');
    expect(reformat('  a\n b\nc'), 'a\nb\nc');
    expect(reformat('  a\n  b\nc'), 'a\nb\nc');
    expect(reformat('  a\n   b\nc'), 'a\nb\nc');
    expect(reformat('  a\n    b\nc'), 'a\n  b\nc');
    expect(reformat('  a\n     b\nc'), 'a\n   b\nc');
  });
  test('Leading indented blocks', () {
    expect(reformat('  a\n  a\nb\nc'), 'a\na\nb\nc');
    expect(reformat('  a\n  a\n b\nc'), 'a\na\nb\nc');
    expect(reformat('  a\n  a\n  b\nc'), 'a\na\nb\nc');
    expect(reformat('  a\n  a\n   b\nc'), 'a\na\nb\nc');
    expect(reformat('  a\n  a\n    b\nc'), 'a\na\n  b\nc');
    expect(reformat('  a\n  a\n     b\nc'), 'a\na\n   b\nc');
  });
  test('Specific cases', () {
    expect(
      reformat('         Apache\n      Version\n   Bla bla\n\nBla bla bla'),
      'Apache\nVersion\nBla bla\n\nBla bla bla',
    );
    expect(
      reformat(
        '/* Copyright (c) IBM Corporation, 2000-2012.  All rights reserved.    */\n'
        '/*                                                                    */\n'
        '/* This software is made available under the terms of the             */\n'
        '/* ICU License -- ICU 1.8.1 and later.                                */\n',
      ),
      'Copyright (c) IBM Corporation, 2000-2012.  All rights reserved.\n'
      '\n'
      'This software is made available under the terms of the\n'
      'ICU License -- ICU 1.8.1 and later.',
    );
    expect(
      reformat(
        '/* Copyright (c) IBM Corporation, 2000-2012.  All rights reserved.    */\n'
        '/*                                                                    */\n'
        '/* This software is made available under the terms of the             */\n'
        '/* ICU License -- ICU 1.8.1 and later.                                */',
      ),
      'Copyright (c) IBM Corporation, 2000-2012.  All rights reserved.\n'
      '\n'
      'This software is made available under the terms of the\n'
      'ICU License -- ICU 1.8.1 and later.',
    );
    expect(
      reformat(
        '/* Copyright (c) IBM Corporation, 2000-2012.  All rights reserved.    */\n'
        '/*                                                                    */\n'
        '/* This software is made available under the terms of the             */\n'
        '/* ICU License -- ICU 1.8.1 and later.',
      ),
      'Copyright (c) IBM Corporation, 2000-2012.  All rights reserved.\n'
      '\n'
      'This software is made available under the terms of the\n'
      'ICU License -- ICU 1.8.1 and later.',
    );
  });
}
