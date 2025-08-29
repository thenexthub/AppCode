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

#ifndef APPCODE_COMMON_MACROS_H_
#define APPCODE_COMMON_MACROS_H_

#if SLIMPELLER

#define ONLY_IN_SLIMPELLER(code) code
#define NOT_SLIMPELLER(code)

#else  // SLIMPELLER

#define ONLY_IN_SLIMPELLER(code)
#define NOT_SLIMPELLER(code) code

#endif  // SLIMPELLER

#endif  // APPCODE_COMMON_MACROS_H_
