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

#ifndef RUNTIME_EFFECT_GLSL_
#define RUNTIME_EFFECT_GLSL_

#if defined(IMPELLER_GRAPHICS_BACKEND)

// Note: The GLES backend uses name matching for attribute locations. This name
// must match the name of the attribute output in:
// impeller/entity/shaders/runtime_effect.vert
in vec2 _fragCoord;
vec2 FlutterFragCoord() {
  return _fragCoord;
}

#elif defined(SKIA_GRAPHICS_BACKEND)

vec2 FlutterFragCoord() {
  return gl_FragCoord.xy;
}

#else
#error "Runtime effect builtins are not supported for this graphics backend."
#endif

#endif
