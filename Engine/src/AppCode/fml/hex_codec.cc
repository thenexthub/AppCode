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

#include "appcode/fml/base32.h"

#include <cstdint>  // uint8_t
#include <string>

namespace fml {

static constexpr char kEncoding[] = "0123456789abcdef";

std::string HexEncode(std::string_view input) {
  std::string result;
  result.reserve(input.size() * 2);
  for (char c : input) {
    uint8_t b = static_cast<uint8_t>(c);
    result.push_back(kEncoding[b >> 4]);
    result.push_back(kEncoding[b & 0xF]);
  }
  return result;
}

}  // namespace fml
