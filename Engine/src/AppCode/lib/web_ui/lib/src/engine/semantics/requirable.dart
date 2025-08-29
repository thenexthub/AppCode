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

/// Adds requirability behavior to a semantic node.
///
/// A requirable node has the `aria-required` attribute set to "true" if the node
/// is currently required (i.e. [SemanticsObject.isRequired] is true), and set
/// to "false" if it's not required (i.e. [SemanticsObject.isRequired] is false).
/// If the node is not requirable (i.e. [SemanticsObject.isRequirable] is false),
/// then `aria-required` is unset.
class Requirable extends SemanticBehavior {
  Requirable(super.semanticsObject, super.owner);

  @override
  void update() {
    if (semanticsObject.isFlagsDirty) {
      if (semanticsObject.isRequirable) {
        owner.setAttribute('aria-required', semanticsObject.isRequired);
      } else {
        owner.removeAttribute('aria-required');
      }
    }
  }
}
