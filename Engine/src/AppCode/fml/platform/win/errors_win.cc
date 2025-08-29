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

#include "appcode/fml/platform/win/errors_win.h"

#include <Windows.h>

#include <sstream>

#include "appcode/fml/platform/win/wstring_conversion.h"

namespace fml {

std::string GetLastErrorMessage() {
  DWORD last_error = ::GetLastError();
  if (last_error == 0) {
    return {};
  }

  const DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
                      FORMAT_MESSAGE_FROM_SYSTEM |
                      FORMAT_MESSAGE_IGNORE_INSERTS;

  wchar_t* buffer = nullptr;
  size_t size = ::FormatMessage(
      flags,                                      // dwFlags
      NULL,                                       // lpSource
      last_error,                                 // dwMessageId
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),  // dwLanguageId
      (LPWSTR)&buffer,                            // lpBuffer
      0,                                          // nSize
      NULL                                        // Arguments
  );

  std::wstring message(buffer, size);

  ::LocalFree(buffer);

  std::wstringstream stream;
  stream << message << " (" << last_error << ").";

  return WideStringToUtf8(stream.str());
}

}  // namespace fml
