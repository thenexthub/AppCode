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

import 'package:meta/meta.dart';

import 'accessibility.dart';
import 'label_and_value.dart';
import 'semantics.dart';

/// Manages semantics configurations that represent live regions.
///
/// A live region is a region whose changes will be announced by the screen
/// reader without the user moving focus onto the node.
///
/// Examples of live regions include snackbars and text field errors. Once
/// identified with this role, they will be able to get the assistive
/// technology's attention right away.
///
/// Different assistive technologies treat "aria-live" attribute differently. To
/// keep the behavior consistent, [AccessibilityAnnouncements.announce] is used.
///
/// When there is an update to [LiveRegion], assistive technologies read the
/// label of the element. See [LabelAndValue]. If there is no label provided
/// no content will be read.
class LiveRegion extends SemanticBehavior {
  LiveRegion(super.semanticsObject, super.owner);

  String? _lastAnnouncement;

  static AccessibilityAnnouncements? _accessibilityAnnouncementsOverride;

  @visibleForTesting
  static void debugOverrideAccessibilityAnnouncements(AccessibilityAnnouncements? value) {
    _accessibilityAnnouncementsOverride = value;
  }

  AccessibilityAnnouncements get _accessibilityAnnouncements =>
      _accessibilityAnnouncementsOverride ?? EngineSemantics.instance.accessibilityAnnouncements;

  @override
  void update() {
    if (!semanticsObject.isLiveRegion) {
      return;
    }

    // Avoid announcing the same message over and over.
    if (_lastAnnouncement != semanticsObject.label) {
      _lastAnnouncement = semanticsObject.label;
      if (semanticsObject.hasLabel) {
        _accessibilityAnnouncements.announce(_lastAnnouncement!, Assertiveness.polite);
      }
    }
  }
}
