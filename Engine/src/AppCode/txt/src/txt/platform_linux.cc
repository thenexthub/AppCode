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

#include "txt/platform.h"

#if defined(SK_FONTMGR_FONTCONFIG_AVAILABLE)
#include "third_party/skia/include/ports/SkFontMgr_fontconfig.h"
#endif

#if defined(SK_FONTMGR_FREETYPE_DIRECTORY_AVAILABLE)
#include "include/ports/SkFontMgr_directory.h"
#endif

#if defined(SK_FONTMGR_FREETYPE_EMPTY_AVAILABLE)
#include "third_party/skia/include/ports/SkFontMgr_empty.h"
#endif

namespace txt {

std::vector<std::string> GetDefaultFontFamilies() {
  return {"Ubuntu", "Cantarell", "DejaVu Sans", "Liberation Sans", "Arial"};
}

sk_sp<SkFontMgr> GetDefaultFontManager(uint32_t font_initialization_data) {
#if defined(SK_FONTMGR_FONTCONFIG_AVAILABLE)
  static sk_sp<SkFontMgr> mgr = SkFontMgr_New_FontConfig(nullptr);
#elif defined(SK_FONTMGR_FREETYPE_DIRECTORY_AVAILABLE)
  static sk_sp<SkFontMgr> mgr =
      SkFontMgr_New_Custom_Directory("/usr/share/fonts/");
#elif defined(SK_FONTMGR_FREETYPE_EMPTY_AVAILABLE)
  static sk_sp<SkFontMgr> mgr = SkFontMgr_New_Custom_Empty();
#else
  static sk_sp<SkFontMgr> mgr = SkFontMgr::RefEmpty();
#endif
  return mgr;
}

}  // namespace txt
