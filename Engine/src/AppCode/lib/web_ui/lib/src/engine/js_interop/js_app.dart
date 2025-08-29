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

import 'dart:js_interop';
import 'package:ui/src/engine.dart';

/// The JS bindings for the configuration object passed to [FlutterApp.addView].
extension type JsFlutterViewOptions._(JSObject _) implements JSObject {
  factory JsFlutterViewOptions() => JSObject() as JsFlutterViewOptions;

  @JS('hostElement')
  external DomElement? get _hostElement;
  DomElement get hostElement {
    assert(_hostElement != null, '`hostElement` passed to addView cannot be null.');
    return _hostElement!;
  }

  external JsViewConstraints? get viewConstraints;
  external JSAny? get initialData;
}

/// The JS bindings for a [ViewConstraints] object.
///
/// Attributes are expressed in *logical* pixels.
extension type JsViewConstraints._(JSObject _) implements JSObject {
  external factory JsViewConstraints({
    double? minWidth,
    double? maxWidth,
    double? minHeight,
    double? maxHeight,
  });

  external double? get maxHeight;
  external double? get maxWidth;
  external double? get minHeight;
  external double? get minWidth;
}

/// The public JS API of a running Flutter Web App.
extension type FlutterApp._primary(JSObject _) implements JSObject {
  factory FlutterApp({
    required AddFlutterViewFn addView,
    required RemoveFlutterViewFn removeView,
  }) => FlutterApp._(addView: addView.toJS, removeView: ((int id) => removeView(id)).toJS);
  external factory FlutterApp._({required JSFunction addView, required JSFunction removeView});
}

/// Typedef for the function that adds a new view to the app.
///
/// Returns the ID of the newly created view.
typedef AddFlutterViewFn = int Function(JsFlutterViewOptions);

/// Typedef for the function that removes a view from the app.
///
/// Returns the configuration used to create the view.
typedef RemoveFlutterViewFn = JsFlutterViewOptions? Function(int);
