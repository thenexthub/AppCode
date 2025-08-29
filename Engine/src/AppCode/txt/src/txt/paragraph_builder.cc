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

#include "paragraph_builder.h"

#include "appcode/txt/src/skia/paragraph_builder_skia.h"
#include "paragraph_style.h"
#include "third_party/icu/source/common/unicode/unistr.h"

namespace txt {

//------------------------------------------------------------------------------
/// @brief      Creates a |ParagraphBuilder| based on Skia's text layout module.
///
/// @param[in]  style             The style to use for the paragraph.
/// @param[in]  font_collection   The font collection to use for the paragraph.
/// @param[in]  impeller_enabled  Whether Impeller is enabled in the runtime.
std::unique_ptr<ParagraphBuilder> ParagraphBuilder::CreateSkiaBuilder(
    const ParagraphStyle& style,
    const std::shared_ptr<FontCollection>& font_collection,
    const bool impeller_enabled) {
  return std::make_unique<ParagraphBuilderSkia>(style, font_collection,
                                                impeller_enabled);
}

}  // namespace txt
