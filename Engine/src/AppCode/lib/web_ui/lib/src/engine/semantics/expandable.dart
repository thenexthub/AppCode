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

import 'semantics.dart';

/// Adds expandability behavior to a semantic node.
///
/// An expandable node would have the `aria-expanded` attribute set to "true" if the node
/// is currently expanded (i.e. [SemanticsObject.isExpanded] is true), and set
/// to "false" if it's not expanded (i.e. [SemanticsObject.isExpanded] is
/// false). If the node is not expandable (i.e. [SemanticsObject.isExpandable]
/// is false), then `aria-expanded` is unset.
class Expandable extends SemanticBehavior {
  Expandable(super.semanticsObject, super.owner);

  @override
  void update() {
    if (semanticsObject.isFlagsDirty) {
      if (semanticsObject.isExpandable) {
        owner.setAttribute('aria-expanded', semanticsObject.isExpanded);
      } else {
        owner.removeAttribute('aria-expanded');
      }
    }
  }
}
