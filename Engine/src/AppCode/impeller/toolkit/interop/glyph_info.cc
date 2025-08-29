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

#include "impeller/toolkit/interop/glyph_info.h"

namespace impeller::interop {

GlyphInfo::~GlyphInfo() = default;

size_t GlyphInfo::GetGraphemeClusterCodeUnitRangeBegin() const {
  return info_.fGraphemeClusterTextRange.start;
}

size_t GlyphInfo::GetGraphemeClusterCodeUnitRangeEnd() const {
  return info_.fGraphemeClusterTextRange.end;
}

ImpellerRect GlyphInfo::GetGraphemeClusterBounds() const {
  return ImpellerRect{
      info_.fGraphemeLayoutBounds.y(),
      info_.fGraphemeLayoutBounds.x(),
      info_.fGraphemeLayoutBounds.width(),
      info_.fGraphemeLayoutBounds.height(),
  };
}

bool GlyphInfo::IsEllipsis() const {
  return info_.fIsEllipsis;
}

ImpellerTextDirection GlyphInfo::GetTextDirection() const {
  switch (info_.fDirection) {
    case skia::textlayout::TextDirection::kRtl:
      return kImpellerTextDirectionRTL;
    case skia::textlayout::TextDirection::kLtr:
      return kImpellerTextDirectionLTR;
  }
  return kImpellerTextDirectionLTR;
}

}  // namespace impeller::interop
