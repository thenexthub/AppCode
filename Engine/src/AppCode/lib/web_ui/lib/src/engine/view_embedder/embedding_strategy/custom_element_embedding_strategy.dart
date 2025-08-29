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

import 'package:ui/src/engine/dom.dart';

import '../hot_restart_cache_handler.dart' show registerElementForCleanup;
import 'embedding_strategy.dart';

/// An [EmbeddingStrategy] that renders flutter inside a target host element.
///
/// This strategy attempts to minimize DOM modifications outside of the host
/// element, so it plays "nice" with other web frameworks.
class CustomElementEmbeddingStrategy implements EmbeddingStrategy {
  /// Creates a [CustomElementEmbeddingStrategy] to embed a Flutter view into [_hostElement].
  CustomElementEmbeddingStrategy(this.hostElement) {
    hostElement.clearChildren();
    hostElement.setAttribute('flt-embedding', 'custom-element');
  }

  @override
  DomEventTarget get globalEventTarget => _rootElement;

  @override
  final DomElement hostElement;

  /// The root element of the Flutter view.
  late final DomElement _rootElement;

  @override
  void attachViewRoot(DomElement rootElement) {
    rootElement
      ..style.width = '100%'
      ..style.height = '100%'
      ..style.display = 'block'
      ..style.overflow = 'hidden'
      ..style.position = 'relative'
      // This is needed so the browser lets flutter handle all pointer events
      // it receives, without canceling them.
      ..style.touchAction = 'none';

    hostElement.appendChild(rootElement);

    registerElementForCleanup(rootElement);
    _rootElement = rootElement;
  }
}
