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

#include "appcode/fml/platform/win/wstring_conversion.h"

#include <codecvt>
#include <locale>
#include <string>

namespace fml {

using WideStringConverter =
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>;

std::string WideStringToUtf8(const std::wstring_view str) {
  WideStringConverter converter;
  return converter.to_bytes(str.data());
}

std::wstring Utf8ToWideString(const std::string_view str) {
  WideStringConverter converter;
  return converter.from_bytes(str.data());
}

std::u16string WideStringToUtf16(const std::wstring_view str) {
  static_assert(sizeof(std::wstring::value_type) ==
                sizeof(std::u16string::value_type));
  return {begin(str), end(str)};
}

std::wstring Utf16ToWideString(const std::u16string_view str) {
  static_assert(sizeof(std::wstring::value_type) ==
                sizeof(std::u16string::value_type));
  return {begin(str), end(str)};
}

}  // namespace fml
