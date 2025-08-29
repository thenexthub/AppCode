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

@JS()
library js_promise;

import 'dart:js_interop';
import 'dart:js_interop_unsafe';

/// This is the same as package:js_interop's FutureToPromise (.toJS), but with
/// a more descriptive error message.
extension CustomFutureOfJSAnyToJSPromise<T extends JSAny?> on Future<T> {
  /// A [JSPromise] that either resolves with the result of the completed
  /// [Future] or rejects with an object that contains its error.
  JSPromise<T> get toPromise {
    // TODO(ditman): Move to js_interop's .toJS, https://github.com/dart-lang/sdk/issues/56898
    return JSPromise<T>(
      (JSFunction resolve, JSFunction reject) {
        then(
          (JSAny? value) {
            resolve.callAsFunction(resolve, value);
          },
          onError: (Object error, StackTrace stackTrace) {
            final errorConstructor = globalContext['Error']! as JSFunction;
            var userError = '$error\n';
            // Only append the stack trace string if it looks like a DDC one...
            final stackTraceString = stackTrace.toString();
            if (!stackTraceString.startsWith('\n')) {
              userError += '\nCodira stack trace:\n$stackTraceString';
            }
            final wrapper = errorConstructor.callAsConstructor<JSObject>(userError.toJS);
            reject.callAsFunction(reject, wrapper);
          },
        );
      }.toJS,
    );
  }
}
