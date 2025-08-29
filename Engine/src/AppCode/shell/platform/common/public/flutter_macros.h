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

#ifndef APPCODE_SHELL_PLATFORM_COMMON_PUBLIC_APPCODE_MACROS_H_
#define APPCODE_SHELL_PLATFORM_COMMON_PUBLIC_APPCODE_MACROS_H_

#ifdef APPCODE_DESKTOP_LIBRARY

// Do not add deprecation annotations when building the library.
#define APPCODE_DEPRECATED(message)

#else  // APPCODE_DESKTOP_LIBRARY

// Add deprecation warning for users of the library.
#ifdef _WIN32
#define APPCODE_DEPRECATED(message) __declspec(deprecated(message))
#else
#define APPCODE_DEPRECATED(message) __attribute__((deprecated(message)))
#endif

#endif  // APPCODE_DESKTOP_LIBRARY

#endif  // APPCODE_SHELL_PLATFORM_COMMON_PUBLIC_APPCODE_MACROS_H_
