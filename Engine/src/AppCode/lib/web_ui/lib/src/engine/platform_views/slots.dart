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

/// Returns the name of a slot from its `viewId`.
///
/// This is used by the [renderContent] function of the [PlatformViewManager]
/// class, and the [createPlatformViewSlot] method below, to keep the slot name
/// attribute consistent across the framework.
String getPlatformViewSlotName(int viewId) {
  return 'flt-pv-slot-$viewId';
}

/// Returns the value of the HTML "id" attribute set on the wrapper element that
/// hosts the platform view content.
String getPlatformViewDomId(int viewId) {
  return 'flt-pv-$viewId';
}

/// Creates the HTML markup for the `slot` of a Platform View.
///
/// The resulting DOM for a `slot` looks like this:
///
/// ```html
/// <flt-platform-view-slot style="...">
///   <slot name="..." />
/// </flt-platform-view-slot>
/// ```
///
/// The inner `SLOT` tag is standard HTML to reveal an element that is rendered
/// elsewhere in the DOM. Its `name` attribute must match the value of the `slot`
/// attribute of the contents being revealed (see [getPlatformViewSlotName].)
///
/// The outer `flt-platform-view-slot` tag is a simple wrapper that the framework
/// can position/style as needed.
///
/// (When the framework accesses a `slot`, it's really accessing its wrapper
/// `flt-platform-view-slot` tag)
DomElement createPlatformViewSlot(int viewId) {
  final String slotName = getPlatformViewSlotName(viewId);

  final DomElement wrapper = domDocument.createElement('flt-platform-view-slot')
    ..style.pointerEvents = 'auto';

  final DomElement slot = domDocument.createElement('slot')..setAttribute('name', slotName);

  return wrapper..append(slot);
}
