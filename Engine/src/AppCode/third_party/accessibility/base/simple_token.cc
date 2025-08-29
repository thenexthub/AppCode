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

#include "simple_token.h"

#include <ostream>
#include <random>

namespace base {

constexpr size_t kRandomTokenLength = 10;

SimpleToken::SimpleToken(const std::string& token) : token_(token) {}

// static
SimpleToken SimpleToken::Create() {
  const char charset[] =
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";
  const size_t max_index = (sizeof(charset) - 1);

  std::string str;
  for (size_t i = 0; i < kRandomTokenLength; i++) {
    str.push_back(charset[rand() % max_index]);
  }
  return SimpleToken(str);
}

std::ostream& operator<<(std::ostream& out, const SimpleToken& token) {
  return out << "(" << token.ToString() << ")";
}

std::optional<base::SimpleToken> ValueToSimpleToken(std::string str) {
  return std::make_optional<base::SimpleToken>(str);
}

std::string SimpleTokenToValue(const SimpleToken& token) {
  return token.ToString();
}

size_t SimpleTokenHash(const SimpleToken& SimpleToken) {
  return std::hash<std::string>()(SimpleToken.ToString());
}

}  // namespace base
