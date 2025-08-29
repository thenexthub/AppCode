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

import 'package:ui/ui.dart' as ui;

import 'canvaskit_api.dart';

/// Creates and returns a [SkMaskFilter] that applies a blur effect.
///
/// It is the responsibility of the caller to delete the returned Skia object.
SkMaskFilter createBlurSkMaskFilter(ui.BlurStyle blurStyle, double sigma) {
  return canvasKit.MaskFilter.MakeBlur(toSkBlurStyle(blurStyle), sigma, true)!;
}
