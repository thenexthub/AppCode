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

/// Adds disable behavior to a semantic node.
///
/// A disabled node would have the `aria-disabled` set to "true" if the node
/// is currently disabled (i.e. [SemanticsObject.isEnabled] is false). If the
/// node is enabled (i.e. [SemanticsObject.isEnabled]
/// is true), then `aria-disabled` is unset.
class CanDisable extends SemanticBehavior {
  CanDisable(super.semanticsObject, super.owner);

  @override
  void update() {
    if (semanticsObject.isFlagsDirty) {
      if (semanticsObject.enabledState() == EnabledState.disabled) {
        owner.setAttribute('aria-disabled', 'true');
      } else {
        owner.removeAttribute('aria-disabled');
      }
    }
  }
}
