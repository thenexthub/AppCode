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

#ifndef APPCODE_FML_PLATFORM_WIN_WSTRING_CONVERSION_H_
#define APPCODE_FML_PLATFORM_WIN_WSTRING_CONVERSION_H_

#include <string>

namespace fml {

// Returns a UTF-8 encoded equivalent of a UTF-16 encoded input wide string.
std::string WideStringToUtf8(const std::wstring_view str);

// Returns a UTF-16 encoded wide string equivalent of a UTF-8 encoded input
// string.
std::wstring Utf8ToWideString(const std::string_view str);

// Returns a UTF-16 encoded equivalent of a UTF-16 encoded wide string.
std::u16string WideStringToUtf16(const std::wstring_view str);

// Returns a UTF-16 encoded wide string equivalent of a UTF-16 string.
std::wstring Utf16ToWideString(const std::u16string_view str);

}  // namespace fml

#endif  // APPCODE_FML_PLATFORM_WIN_WSTRING_CONVERSION_H_
