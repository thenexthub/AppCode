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

/// Sets global attributes for a Flutter View.
///
/// The global attributes are set on the [rootElement] of the Flutter View, as
/// well as the on [hostElement] where the Flutter View is inserted.
///
/// The [hostElement] corresponds to the <body> element in full-page mode.
///
/// The global attributes provide quick and general information about the
/// Flutter app. They are set on a global element (e.g. the body element) to
/// make it easily accessible to the user.
class GlobalHtmlAttributes {
  GlobalHtmlAttributes({required this.rootElement, required this.hostElement});

  /// The [FlutterView.viewId] attribute name.
  static const String flutterViewIdAttributeName = 'flt-view-id';

  final DomElement rootElement;
  final DomElement hostElement;

  void applyAttributes({
    required int viewId,
    required String rendererTag,
    required String buildMode,
  }) {
    // This `flt-view-id` attribute does not serve a function in the engine's
    // operation, but it's useful for debugging, test automation, and DOM
    // interop use-cases. It allows one to use CSS selectors to find views by
    // their identifiers.
    //
    // Example:
    //
    //     document.querySelector('flutter-view[flt-view-id="$viewId"]')
    rootElement.setAttribute(flutterViewIdAttributeName, viewId);

    hostElement.setAttribute('flt-renderer', rendererTag);
    hostElement.setAttribute('flt-build-mode', buildMode);
    // TODO(mdebbar): Disable spellcheck until changes in the framework and
    // engine are complete.
    hostElement.setAttribute('spellcheck', 'false');
  }
}
