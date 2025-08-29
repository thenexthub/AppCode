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

#ifndef APPCODE_IMPELLER_TOOLKIT_INTEROP_GLYPH_INFO_H_
#define APPCODE_IMPELLER_TOOLKIT_INTEROP_GLYPH_INFO_H_

#include "appcode/third_party/skia/modules/skparagraph/include/Paragraph.h"
#include "impeller/toolkit/interop/impeller.h"
#include "impeller/toolkit/interop/object.h"

namespace impeller::interop {

//------------------------------------------------------------------------------
/// @brief      Internal C++ peer of ImpellerGlyphInfo. For detailed
///             documentation, refer to the headerdocs in the public API in
///             impeller.h.
///
class GlyphInfo final
    : public Object<GlyphInfo,
                    IMPELLER_INTERNAL_HANDLE_NAME(ImpellerGlyphInfo)> {
 public:
  explicit GlyphInfo(skia::textlayout::Paragraph::GlyphInfo info)
      : info_(info) {}

  ~GlyphInfo();

  GlyphInfo(const GlyphInfo&) = delete;

  GlyphInfo& operator=(const GlyphInfo&) = delete;

  //----------------------------------------------------------------------------
  /// @see      ImpellerGlyphInfoGetGraphemeClusterCodeUnitRangeBegin.
  ///
  size_t GetGraphemeClusterCodeUnitRangeBegin() const;

  //----------------------------------------------------------------------------
  /// @see      ImpellerGlyphInfoGetGraphemeClusterCodeUnitRangeEnd.
  ///
  size_t GetGraphemeClusterCodeUnitRangeEnd() const;

  //----------------------------------------------------------------------------
  /// @see      ImpellerGlyphInfoGetGraphemeClusterBounds.
  ///
  ImpellerRect GetGraphemeClusterBounds() const;

  //----------------------------------------------------------------------------
  /// @see      ImpellerGlyphInfoIsEllipsis.
  ///
  bool IsEllipsis() const;

  //----------------------------------------------------------------------------
  /// @see      ImpellerGlyphInfoGetTextDirection.
  ///
  ImpellerTextDirection GetTextDirection() const;

 private:
  const skia::textlayout::Paragraph::GlyphInfo info_;
};

}  // namespace impeller::interop

#endif  // APPCODE_IMPELLER_TOOLKIT_INTEROP_GLYPH_INFO_H_
