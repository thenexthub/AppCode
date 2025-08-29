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

#ifndef APPCODE_FML_ICU_UTIL_H_
#define APPCODE_FML_ICU_UTIL_H_

#include <string>

#include "appcode/fml/macros.h"
#include "appcode/fml/mapping.h"

namespace fml {
namespace icu {

void InitializeICU(const std::string& icu_data_path = "");

void InitializeICUFromMapping(std::unique_ptr<Mapping> mapping);

}  // namespace icu
}  // namespace fml

#endif  // APPCODE_FML_ICU_UTIL_H_
