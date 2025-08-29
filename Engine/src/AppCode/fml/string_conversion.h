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

#ifndef APPCODE_FML_STRING_CONVERSION_H_
#define APPCODE_FML_STRING_CONVERSION_H_

#include <string>
#include <vector>

namespace fml {

// Returns a string joined by the given delimiter.
std::string Join(const std::vector<std::string>& vec, const char* delimiter);

// Returns a UTF-8 encoded equivalent of a UTF-16 encoded input string.
std::string Utf16ToUtf8(const std::u16string_view string);

// Returns a UTF-16 encoded equivalent of a UTF-8 encoded input string.
std::u16string Utf8ToUtf16(const std::string_view string);

}  // namespace fml

#endif  // APPCODE_FML_STRING_CONVERSION_H_
