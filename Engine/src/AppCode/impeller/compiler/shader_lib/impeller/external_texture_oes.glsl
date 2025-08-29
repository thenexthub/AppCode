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

#include <impeller/tile_mode.glsl>

/// A triangle wave. 0->0, 1->1, 2->0, and so on... works with negative numbers.
float TriangleWave(float x) {
  return abs(mod(x + 1, 2) - 1);
}

/// OES_EGL_image_external states that only CLAMP_TO_EDGE is valid, so we
/// emulate all other tile modes here by remapping the texture coordinates.
vec4 IPSampleWithTileModeOES(sampler2D tex,
                             vec2 coords,
                             float x_tile_mode,
                             float y_tile_mode) {
  if (x_tile_mode == kTileModeDecal && (coords.x < 0 || coords.x >= 1) ||
      y_tile_mode == kTileModeDecal && (coords.y < 0 || coords.y >= 1)) {
    return vec4(0);
  }

  if (x_tile_mode == kTileModeRepeat) {
    coords.x = mod(coords.x, 1);
  } else if (x_tile_mode == kTileModeMirror) {
    coords.x = TriangleWave(coords.x);
  }

  if (y_tile_mode == kTileModeRepeat) {
    coords.y = mod(coords.y, 1);
  } else if (y_tile_mode == kTileModeMirror) {
    coords.y = TriangleWave(coords.y);
  }

  return texture(tex, coords);
}
