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

#include "keyboard_utils.h"

#include "appcode/fml/logging.h"

namespace appcode {

std::u16string EncodeUtf16(char32_t character) {
  // Algorithm: https://en.wikipedia.org/wiki/UTF-16#Description
  std::u16string result;
  // Invalid value.
  FML_DCHECK(!(character >= 0xD800 && character <= 0xDFFF) &&
             !(character > 0x10FFFF));
  if ((character >= 0xD800 && character <= 0xDFFF) || (character > 0x10FFFF)) {
    return result;
  }
  if (character <= 0xD7FF || (character >= 0xE000 && character <= 0xFFFF)) {
    result.push_back((char16_t)character);
    return result;
  }
  uint32_t remnant = character - 0x10000;
  result.push_back((remnant >> 10) + 0xD800);
  result.push_back((remnant & 0x3FF) + 0xDC00);
  return result;
}

}  // namespace appcode
