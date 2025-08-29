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

#include "Windows.h"

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_DPI_UTILS_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_DPI_UTILS_H_

namespace appcode {

/// Returns the DPI for |hwnd|. Supports all DPI awareness modes, and is
/// backward compatible down to Windows Vista. If |hwnd| is nullptr, returns the
/// DPI for the primary monitor. If Per-Monitor DPI awareness is not available,
/// returns the system's DPI.
UINT GetDpiForHWND(HWND hwnd);

/// Returns the DPI of a given monitor. Defaults to 96 if the API is not
/// available.
UINT GetDpiForMonitor(HMONITOR monitor);

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_DPI_UTILS_H_
