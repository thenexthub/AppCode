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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_WINDOWSX_SHIM_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_WINDOWSX_SHIM_H_

// The Win32 platform header <windowsx.h> contains some macros for
// common function names. To work around that, windowsx.h is not to be
// included directly, and instead this file should be included. If one
// of the removed Win32 macros is wanted, use the expanded form
// manually instead.

#ifdef _INC_WINDOWS_X
#error "There is an include of windowsx.h in the code. Use windowsx_shim.h"
#endif  // _INC_WINDOWS_X

#include <windowsx.h>

#undef GetNextSibling  // Same as GetWindow(hwnd, GW_HWNDNEXT)
#undef GetFirstChild   // Same as GetTopWindow(hwnd)
#undef IsMaximized     // Defined to IsZoomed, use IsZoomed directly instead
#undef IsMinimized     // Defined to IsIconic, use IsIconic directly instead
#undef IsRestored      // Macro to check that neither WS_MINIMIZE, nor
                       // WS_MAXIMIZE is set in the GetWindowStyle return
                       // value.

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_WINDOWSX_SHIM_H_
