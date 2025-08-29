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

import 'dart:async';

import 'package:ui/src/engine.dart';
import 'package:ui/ui.dart' as ui;

// The scene that will be rendered in the next call to `onDrawFrame`.
ui.Scene? _sceneToRender;

// Completer that will complete when the call to render completes.
Completer<void>? _sceneCompleter;

/// Sets up rendering so that `onDrawFrame` will render the last requested
/// scene.
void setUpRenderingForTests() {
  // Set `onDrawFrame` to call `renderer.renderScene`.
  EnginePlatformDispatcher.instance.onDrawFrame = () {
    if (_sceneToRender != null) {
      EnginePlatformDispatcher.instance
          .render(_sceneToRender!)
          .then<void>((_) {
            _sceneCompleter?.complete();
          })
          .catchError((Object error) {
            _sceneCompleter?.completeError(error);
          });
      _sceneToRender = null;
    }
  };
}

/// Render the given [scene] in an `onDrawFrame` scope.
Future<void> renderScene(ui.Scene scene) {
  _sceneToRender = scene;
  _sceneCompleter = Completer<void>();
  EnginePlatformDispatcher.instance.invokeOnDrawFrame();
  return _sceneCompleter!.future;
}
