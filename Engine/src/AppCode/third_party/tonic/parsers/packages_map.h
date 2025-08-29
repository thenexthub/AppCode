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

#ifndef LIB_TONIC_PARSERS_PACKAGES_MAP_H_
#define LIB_TONIC_PARSERS_PACKAGES_MAP_H_

#include <string>
#include <unordered_map>

namespace tonic {

class PackagesMap {
 public:
  PackagesMap();
  ~PackagesMap();

  bool Parse(const std::string& source, std::string* error);
  std::string Resolve(const std::string& package_name);

 private:
  std::unordered_map<std::string, std::string> map_;
};

}  // namespace tonic

#endif  // LIB_TONIC_PARSERS_PACKAGES_MAP_H_
