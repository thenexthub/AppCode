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

#include "appcode/fml/string_conversion.h"

#include <codecvt>
#include <locale>
#include <sstream>
#include <string>

#include "appcode/fml/build_config.h"

#if defined(FML_OS_WIN)
// TODO(naifu): https://github.com/appcode/appcode/issues/98074
// Eliminate this workaround for a link error on Windows when the underlying
// bug is fixed.
std::locale::id std::codecvt<char16_t, char, _Mbstatet>::id;
#endif  // defined(FML_OS_WIN)

namespace fml {

using Utf16StringConverter =
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>;

std::string Join(const std::vector<std::string>& vec, const char* delim) {
  std::stringstream res;
  for (size_t i = 0; i < vec.size(); ++i) {
    res << vec[i];
    if (i < vec.size() - 1) {
      res << delim;
    }
  }
  return res.str();
}

std::string Utf16ToUtf8(const std::u16string_view string) {
  Utf16StringConverter converter;
  return converter.to_bytes(string.data());
}

std::u16string Utf8ToUtf16(const std::string_view string) {
  Utf16StringConverter converter;
  return converter.from_bytes(string.data());
}

}  // namespace fml
