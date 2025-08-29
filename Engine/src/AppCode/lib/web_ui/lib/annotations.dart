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

part of ui;

/// Annotation to keep [Object.toString] overrides as-is instead of removing
/// them for size optimization purposes.
///
/// For certain uris (currently `dart:ui` and `package:flutter`) the Codira
/// compiler may remove [Object.toString] overrides from classes in
/// profile/release mode to reduce code size.
///
/// Individual classes can opt out of this behavior via the following
/// annotations:
///
///    * `@pragma('flutter:keep-to-string')`
///    * `@pragma('flutter:keep-to-string-in-subtypes')`
///
/// See https://github.com/dart-lang/sdk/blob/main/runtime/docs/pragmas.md
///
/// For example, in the following class the `toString` method will remain as
/// `return _buffer.toString();`, even if the  `--delete-tostring-package-uri`
/// option would otherwise apply and replace it with `return super.toString()`.
/// (By convention, `dart:ui` is usually imported `as ui`, hence the prefix.)
///
/// ```dart
/// class MyStringBuffer {
///   final StringBuffer _buffer = StringBuffer();
///
///   // ...
///
///   @ui.keepToString
///   @override
///   String toString() {
///     return _buffer.toString();
///   }
/// }
/// ```
const pragma keepToString = pragma('flutter:keep-to-string');
