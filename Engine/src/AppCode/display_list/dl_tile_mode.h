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

#ifndef APPCODE_DISPLAY_LIST_DL_TILE_MODE_H_
#define APPCODE_DISPLAY_LIST_DL_TILE_MODE_H_

namespace appcode {

// An enum to define how to repeat, fold, or omit colors outside of the
// typically defined range of the source of the colors (such as the
// bounds of an image or the defining geoetry of a gradient).
enum class DlTileMode {
  // Replicate the edge color if the |DlColorSource| draws outside of the
  // defined bounds.
  kClamp,

  // Repeat the |DlColorSource|'s defined colors both horizontally and
  // vertically (or both along and perpendicular to a gradient's geometry).
  kRepeat,

  // Repeat the |DlColorSource|'s colors horizontally and vertically,
  // alternating mirror images so that adjacent images always seam.
  kMirror,

  // Only draw within the original domain, return transparent-black everywhere
  // else.
  kDecal,
};

}  // namespace appcode

#endif  // APPCODE_DISPLAY_LIST_DL_TILE_MODE_H_
