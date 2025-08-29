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

#ifndef BASE_COLOR_UTILS_H_
#define BASE_COLOR_UTILS_H_

/** Returns alpha byte from color value.
 */
#define ColorGetA(color) (((color) >> 24) & 0xFF)

/** Returns red component of color, from zero to 255.
 */
#define ColorGetR(color) (((color) >> 16) & 0xFF)

/** Returns green component of color, from zero to 255.
 */
#define ColorGetG(color) (((color) >> 8) & 0xFF)

/** Returns blue component of color, from zero to 255.
 */
#define ColorGetB(color) (((color) >> 0) & 0xFF)

#endif  // BASE_COLOR_UTILS_H_
