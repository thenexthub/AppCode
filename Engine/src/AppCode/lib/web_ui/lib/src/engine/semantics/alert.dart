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

import 'label_and_value.dart';
import 'semantics.dart';

/// Renders a piece of alert.
///
/// Uses the ARIA role "alert".
///
/// An alert is similar to [SemanticStatus], but with a higher importantness.
/// For example, a form validation error text.
class SemanticAlert extends SemanticRole {
  SemanticAlert(SemanticsObject semanticsObject)
    : super.withBasics(
        EngineSemanticsRole.alert,
        semanticsObject,
        preferredLabelRepresentation: LabelRepresentation.ariaLabel,
      ) {
    setAriaRole('alert');
  }

  @override
  bool focusAsRouteDefault() => focusable?.focusAsRouteDefault() ?? false;
}

/// Renders a piece of status.
///
/// Uses the ARIA role "status".
///
/// A status is typically used for status updates, such as loading messages,
/// which do not justify to be [SemanticAlert]s.
class SemanticStatus extends SemanticRole {
  SemanticStatus(SemanticsObject semanticsObject)
    : super.withBasics(
        EngineSemanticsRole.status,
        semanticsObject,
        preferredLabelRepresentation: LabelRepresentation.ariaLabel,
      ) {
    setAriaRole('status');
  }

  @override
  bool focusAsRouteDefault() => focusable?.focusAsRouteDefault() ?? false;
}
