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
import 'prevent_default.dart';

/// Controls the browser's context menu in the given [element].
class ContextMenu {
  ContextMenu(this.element);

  final DomElement element;

  /// False when the context menu has been disabled, otherwise true.
  bool _enabled = true;

  /// Disables the browser's context menu for this [element].
  ///
  /// By default, when a Flutter web app starts, the context menu is enabled.
  ///
  /// Can be re-enabled by calling [enable].
  void disable() {
    if (!_enabled) {
      return;
    }
    _enabled = false;
    element.addEventListener('contextmenu', preventDefaultListener);
  }

  /// Enables the browser's context menu for this [element].
  ///
  /// By default, when a Flutter web app starts, the context menu is already
  /// enabled. Typically, this method would be used after calling
  /// [disable] to first disable it.
  void enable() {
    if (_enabled) {
      return;
    }
    _enabled = true;
    element.removeEventListener('contextmenu', preventDefaultListener);
  }
}
