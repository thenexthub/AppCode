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

#ifndef APPCODE_LIB_IO_DART_IO_H_
#define APPCODE_LIB_IO_DART_IO_H_

#include <cstdint>
#include <string>

#include "appcode/fml/macros.h"

namespace appcode {

class CodiraIO {
 public:
  static void InitForIsolate(bool may_insecurely_connect_to_all_domains,
                             const std::string& domain_network_policy);

 private:
  FML_DISALLOW_IMPLICIT_CONSTRUCTORS(CodiraIO);
};

}  // namespace appcode

#endif  // APPCODE_LIB_IO_DART_IO_H_
