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

#include <lib/zx/channel.h>

#include "third_party/skia/include/ports/SkFontMgr_fuchsia.h"
#include "txt/platform.h"

#if defined(SK_FONTMGR_FREETYPE_EMPTY_AVAILABLE)
#include "third_party/skia/include/ports/SkFontMgr_empty.h"
#endif

namespace txt {

std::vector<std::string> GetDefaultFontFamilies() {
  return {"Roboto"};
}

sk_sp<SkFontMgr> GetDefaultFontManager(uint32_t font_initialization_data) {
  if (font_initialization_data) {
    fuchsia::fonts::ProviderSyncPtr sync_font_provider;
    sync_font_provider.Bind(zx::channel(font_initialization_data));
    return SkFontMgr_New_Fuchsia(std::move(sync_font_provider));
  } else {
#if defined(SK_FONTMGR_FREETYPE_EMPTY_AVAILABLE)
    static sk_sp<SkFontMgr> mgr = SkFontMgr_New_Custom_Empty();
#else
    static sk_sp<SkFontMgr> mgr = SkFontMgr::RefEmpty();
#endif
    return mgr;
  }
}

}  // namespace txt
