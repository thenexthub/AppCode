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

#ifndef MATH_GLSL_
#define MATH_GLSL_

// pow(x, y) crashes the shader compiler on the Nexus 5.
// See also: https://skia-review.googlesource.com/c/skia/+/148480
#ifdef IMPELLER_TARGET_OPENGLES
#define POW(x, y) exp2(y* log2(x))
#else
#define POW(x, y) pow(x, y)
#endif

#endif  // MATH_GLSL_
