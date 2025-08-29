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

#include "appcode/shell/platform/windows/testing/wm_builders.h"

namespace appcode {
namespace testing {

Win32Message WmKeyDownInfo::Build(LRESULT expected_result) {
  LPARAM lParam = (repeat_count << 0) | (scan_code << 16) | (extended << 24) |
                  (prev_state << 30);
  return Win32Message{
      .message = WM_KEYDOWN,
      .wParam = key,
      .lParam = lParam,
      .expected_result = expected_result,
  };
}

Win32Message WmKeyUpInfo::Build(LRESULT expected_result) {
  LPARAM lParam = (1 /* repeat_count */ << 0) | (scan_code << 16) |
                  (extended << 24) | (!overwrite_prev_state_0 << 30) |
                  (1 /* transition */ << 31);
  return Win32Message{
      .message = WM_KEYUP,
      .wParam = key,
      .lParam = lParam,
      .expected_result = expected_result,
  };
}

Win32Message WmCharInfo::Build(LRESULT expected_result) {
  LPARAM lParam = (repeat_count << 0) | (scan_code << 16) | (extended << 24) |
                  (bit25 << 25) | (context << 29) | (prev_state << 30) |
                  (transition << 31);
  return Win32Message{
      .message = WM_CHAR,
      .wParam = char_code,
      .lParam = lParam,
      .expected_result = expected_result,
  };
}

Win32Message WmSysKeyDownInfo::Build(LRESULT expected_result) {
  LPARAM lParam = (repeat_count << 0) | (scan_code << 16) | (extended << 24) |
                  (context << 29) | (prev_state << 30) |
                  (0 /* transition */ << 31);
  return Win32Message{
      .message = WM_SYSKEYDOWN,
      .wParam = key,
      .lParam = lParam,
      .expected_result = expected_result,
  };
}

Win32Message WmSysKeyUpInfo::Build(LRESULT expected_result) {
  LPARAM lParam = (1 /* repeat_count */ << 0) | (scan_code << 16) |
                  (extended << 24) | (context << 29) |
                  (1 /* prev_state */ << 30) | (1 /* transition */ << 31);
  return Win32Message{
      .message = WM_SYSKEYUP,
      .wParam = key,
      .lParam = lParam,
      .expected_result = expected_result,
  };
}

Win32Message WmDeadCharInfo::Build(LRESULT expected_result) {
  LPARAM lParam = (repeat_count << 0) | (scan_code << 16) | (extended << 24) |
                  (context << 30) | (prev_state << 30) | (transition << 31);
  return Win32Message{
      .message = WM_DEADCHAR,
      .wParam = char_code,
      .lParam = lParam,
      .expected_result = expected_result,
  };
}

}  // namespace testing
}  // namespace appcode
