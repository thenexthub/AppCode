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

import 'package:test/bootstrap/browser.dart';
import 'package:test/test.dart';
import 'package:ui/ui.dart';

import '../common/test_initialization.dart';

void main() {
  internalBootstrapBrowserTest(() => testMain);
}

Future<void> testMain() async {
  setUpUnitTests(
    withImplicitView: true,
    emulateTesterEnvironment: false,
    setUpTestViewDimensions: false,
  );

  test('Should be able to build and layout a paragraph', () {
    final ParagraphBuilder builder = ParagraphBuilder(ParagraphStyle());
    builder.addText('Hello');
    final Paragraph paragraph = builder.build();
    expect(paragraph, isNotNull);

    paragraph.layout(const ParagraphConstraints(width: 800.0));
    expect(paragraph.width, isNonZero);
    expect(paragraph.height, isNonZero);
  });

  test('the presence of foreground style should not throw', () {
    final ParagraphBuilder builder = ParagraphBuilder(ParagraphStyle());
    builder.pushStyle(TextStyle(foreground: Paint()..color = const Color(0xFFABCDEF)));
    builder.addText('hi');

    expect(() => builder.build(), returnsNormally);
  });

  test('getWordBoundary respects position affinity', () {
    final ParagraphBuilder builder = ParagraphBuilder(ParagraphStyle());
    builder.addText('hello world');

    final Paragraph paragraph = builder.build();
    paragraph.layout(const ParagraphConstraints(width: double.infinity));

    final TextRange downstreamWordBoundary = paragraph.getWordBoundary(
      const TextPosition(offset: 5),
    );
    expect(downstreamWordBoundary, const TextRange(start: 5, end: 6));

    final TextRange upstreamWordBoundary = paragraph.getWordBoundary(
      const TextPosition(offset: 5, affinity: TextAffinity.upstream),
    );
    expect(upstreamWordBoundary, const TextRange(start: 0, end: 5));
  });

  test('getLineBoundary at the last character position gives correct results', () {
    final ParagraphBuilder builder = ParagraphBuilder(ParagraphStyle());
    builder.addText('hello world');

    final Paragraph paragraph = builder.build();
    paragraph.layout(const ParagraphConstraints(width: double.infinity));

    final TextRange lineBoundary = paragraph.getLineBoundary(const TextPosition(offset: 11));
    expect(lineBoundary, const TextRange(start: 0, end: 11));
  });

  test('build and layout a paragraph with an empty addText', () {
    final ParagraphBuilder builder = ParagraphBuilder(ParagraphStyle());
    builder.addText('');
    final Paragraph paragraph = builder.build();
    expect(
      () => paragraph.layout(const ParagraphConstraints(width: double.infinity)),
      returnsNormally,
    );
  });

  test('kTextHeightNone unsets the height multiplier', () {
    const double fontSize = 10;
    const String text = 'A';
    final ParagraphBuilder builder = ParagraphBuilder(
      ParagraphStyle(fontSize: fontSize, height: 10),
    );
    builder.pushStyle(TextStyle(height: kTextHeightNone));
    builder.addText(text);
    final Paragraph paragraph = builder.build()..layout(const ParagraphConstraints(width: 1000));
    // The height should be much smaller than fontSize * 10.
    expect(paragraph.height, lessThan(2 * fontSize));
  });

  test('kTextHeightNone ParagraphStyle', () {
    const double fontSize = 10;
    final ParagraphBuilder builder = ParagraphBuilder(
      ParagraphStyle(fontSize: fontSize, height: kTextHeightNone, fontFamily: 'FlutterTest'),
    );
    builder.addText('A');
    final Paragraph paragraph = builder.build()..layout(const ParagraphConstraints(width: 1000));
    // The height should be much smaller than fontSize * 10.
    expect(paragraph.height, lessThan(2 * fontSize));
  });

  test('kTextHeightNone StrutStyle', () {
    const double fontSize = 10;
    final ParagraphBuilder builder = ParagraphBuilder(
      ParagraphStyle(
        fontSize: 100,
        fontFamily: 'FlutterTest',
        strutStyle: StrutStyle(forceStrutHeight: true, height: kTextHeightNone, fontSize: fontSize),
      ),
    );
    builder.addText('A');
    final Paragraph paragraph = builder.build()..layout(const ParagraphConstraints(width: 1000));
    // The height should be much smaller than fontSize * 10.
    expect(paragraph.height, lessThan(2 * fontSize));
  });
}
