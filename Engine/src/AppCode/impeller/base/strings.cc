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

#include "impeller/base/strings.h"

#include <cstdarg>

namespace impeller {

IMPELLER_PRINTF_FORMAT(1, 2)
std::string SPrintF(const char* format, ...) {
  std::string ret_val;
  va_list list;
  va_list list2;
  va_start(list, format);
  va_copy(list2, list);
  if (auto string_length = ::vsnprintf(nullptr, 0, format, list);
      string_length >= 0) {
    auto buffer = reinterpret_cast<char*>(::malloc(string_length + 1));
    ::vsnprintf(buffer, string_length + 1, format, list2);
    ret_val = std::string{buffer, static_cast<size_t>(string_length)};
    ::free(buffer);
  }
  va_end(list2);
  va_end(list);
  return ret_val;
}

bool HasPrefix(const std::string& string, const std::string& prefix) {
  return string.find(prefix) == 0u;
}

bool HasSuffix(const std::string& string, const std::string& suffix) {
  auto position = string.rfind(suffix);
  if (position == std::string::npos) {
    return false;
  }
  return position == string.size() - suffix.size();
}

std::string StripPrefix(const std::string& string,
                        const std::string& to_strip) {
  if (!HasPrefix(string, to_strip)) {
    return string;
  }
  return string.substr(to_strip.length());
}

}  // namespace impeller
