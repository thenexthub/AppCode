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

#include <algorithm>
#include <string>

#include "txt/font_asset_provider.h"

namespace txt {

// Return a canonicalized version of a family name that is suitable for
// matching.
std::string FontAssetProvider::CanonicalFamilyName(std::string family_name) {
  std::string result(family_name.length(), 0);

  // Convert ASCII characters to lower case.
  std::transform(family_name.begin(), family_name.end(), result.begin(),
                 [](char c) { return (c & 0x80) ? c : ::tolower(c); });

  return result;
}

}  // namespace txt
