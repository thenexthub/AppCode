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

class NotoFont {
  NotoFont(this.name, this.url);

  final String name;
  final String url;

  final int index = _index++;
  static int _index = 0;

  /// During fallback font selection this is the number of missing code points
  /// that are covered by (i.e. in) this font.
  int coverCount = 0;

  /// During fallback font selection this is a list of [FallbackFontComponent]s
  /// from this font that are required to cover some of the missing code
  /// points. The cover count for the font is the sum of the cover counts for
  /// the components that make up the font.
  final List<FallbackFontComponent> coverComponents = <FallbackFontComponent>[];
}

/// A component is a set of code points common to some fonts. Each code point is
/// in a single component. Each font can be represented as a disjoint union of
/// components. We store the inverse of this relationship, the fonts that use
/// this component. The font fallback selection algorithm does not need the code
/// points in a component or a font, so this is not stored, but can be recovered
/// via the map from code-point to component.
class FallbackFontComponent {
  FallbackFontComponent(this.fonts);
  final List<NotoFont> fonts;

  /// During fallback font selection this is the number of missing code points
  /// that are covered by this component.
  int coverCount = 0;
}
