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

#include "impeller/compiler/uniform_sorter.h"

#include <cstdint>

namespace impeller {

std::vector<spirv_cross::ID> SortUniforms(
    const spirv_cross::ParsedIR* ir,
    const spirv_cross::Compiler* compiler,
    std::optional<spirv_cross::SPIRType::BaseType> type_filter,
    bool include) {
  // Sort the IR so that the uniforms are in declaration order.
  std::vector<spirv_cross::ID> uniforms;
  ir->for_each_typed_id<spirv_cross::SPIRVariable>(
      [&](uint32_t, const spirv_cross::SPIRVariable& var) {
        if (var.storage != spv::StorageClassUniformConstant) {
          return;
        }
        const auto& type = compiler->get_type(var.basetype);
        if (!type_filter.has_value() ||
            (include && type_filter.value() == type.basetype) ||
            (!include && type_filter.value() != type.basetype)) {
          uniforms.push_back(var.self);
        }
      });

  auto compare_locations = [&ir](spirv_cross::ID id1, spirv_cross::ID id2) {
    auto& flags1 = ir->get_decoration_bitset(id1);
    auto& flags2 = ir->get_decoration_bitset(id2);
    // Put the uniforms with no location after the ones that have a location.
    if (!flags1.get(spv::Decoration::DecorationLocation)) {
      return false;
    }
    if (!flags2.get(spv::Decoration::DecorationLocation)) {
      return true;
    }
    // Sort in increasing order of location.
    return ir->get_decoration(id1, spv::Decoration::DecorationLocation) <
           ir->get_decoration(id2, spv::Decoration::DecorationLocation);
  };
  std::sort(uniforms.begin(), uniforms.end(), compare_locations);

  return uniforms;
}

}  // namespace impeller
