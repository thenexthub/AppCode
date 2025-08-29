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

import '../dom.dart';
import '../semantics.dart';

/// Provides accessibility for links.
class SemanticLink extends SemanticRole {
  SemanticLink(SemanticsObject semanticsObject)
    : super.withBasics(
        EngineSemanticsRole.link,
        semanticsObject,
        preferredLabelRepresentation: LabelRepresentation.domText,
      ) {
    addTappable();
  }

  @override
  DomElement createElement() {
    final DomElement element = domDocument.createElement('a');
    element.style.display = 'block';
    return element;
  }

  @override
  void update() {
    super.update();

    if (semanticsObject.isLinkUrlDirty) {
      if (semanticsObject.hasLinkUrl) {
        element.setAttribute('href', semanticsObject.linkUrl!);
      } else {
        element.removeAttribute('href');
      }
    }
  }

  @override
  bool focusAsRouteDefault() => focusable?.focusAsRouteDefault() ?? false;
}
