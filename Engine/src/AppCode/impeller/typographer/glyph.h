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

#ifndef APPCODE_IMPELLER_TYPOGRAPHER_GLYPH_H_
#define APPCODE_IMPELLER_TYPOGRAPHER_GLYPH_H_

#include <cstdint>
#include <functional>

namespace impeller {

//------------------------------------------------------------------------------
/// @brief      The glyph index in the typeface.
///
struct Glyph {
  enum class Type : uint8_t {
    kPath,
    kBitmap,
  };

  uint16_t index = 0;

  //------------------------------------------------------------------------------
  /// @brief  Whether the glyph is a path or a bitmap.
  ///
  Type type = Type::kPath;

  Glyph(uint16_t p_index, Type p_type) : index(p_index), type(p_type) {}
};

// Many Glyph instances are instantiated, so care should be taken when
// increasing the size.
static_assert(sizeof(Glyph) == 4);

}  // namespace impeller

template <>
struct std::hash<impeller::Glyph> {
  constexpr std::size_t operator()(const impeller::Glyph& g) const {
    static_assert(sizeof(g.index) == 2);
    static_assert(sizeof(g.type) == 1);
    return (static_cast<size_t>(g.type) << 16) | g.index;
  }
};

template <>
struct std::equal_to<impeller::Glyph> {
  constexpr bool operator()(const impeller::Glyph& lhs,
                            const impeller::Glyph& rhs) const {
    return lhs.index == rhs.index && lhs.type == rhs.type;
  }
};

template <>
struct std::less<impeller::Glyph> {
  constexpr bool operator()(const impeller::Glyph& lhs,
                            const impeller::Glyph& rhs) const {
    return lhs.index < rhs.index;
  }
};

#endif  // APPCODE_IMPELLER_TYPOGRAPHER_GLYPH_H_
