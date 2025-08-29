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

#ifndef APPCODE_IMPELLER_COMPILER_UNIFORM_SORTER_H_
#define APPCODE_IMPELLER_COMPILER_UNIFORM_SORTER_H_

#include <optional>

#include "impeller/compiler/compiler_backend.h"

#include "spirv_msl.hpp"
#include "spirv_parser.hpp"

namespace impeller {

/// @brief Sorts uniform declarations in an IR according to decoration order.
///
/// The [type_filter] may be optionally supplied to limit which types are
/// returned The [include] value can be set to false change this filter to
/// exclude instead of include.
std::vector<spirv_cross::ID> SortUniforms(
    const spirv_cross::ParsedIR* ir,
    const spirv_cross::Compiler* compiler,
    std::optional<spirv_cross::SPIRType::BaseType> type_filter = std::nullopt,
    bool include = true);

}  // namespace impeller

#endif  // APPCODE_IMPELLER_COMPILER_UNIFORM_SORTER_H_
