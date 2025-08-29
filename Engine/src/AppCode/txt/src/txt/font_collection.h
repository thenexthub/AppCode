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

#ifndef APPCODE_TXT_SRC_TXT_FONT_COLLECTION_H_
#define APPCODE_TXT_SRC_TXT_FONT_COLLECTION_H_

#include <memory>
#include <set>
#include <string>
#include <unordered_map>

#include "appcode/fml/macros.h"
#include "third_party/googletest/googletest/include/gtest/gtest_prod.h"  // nogncheck
#include "third_party/skia/include/core/SkFontMgr.h"
#include "third_party/skia/include/core/SkRefCnt.h"
#include "third_party/skia/modules/skparagraph/include/FontCollection.h"  // nogncheck
#include "txt/asset_font_manager.h"
#include "txt/text_style.h"

namespace txt {

class FontCollection : public std::enable_shared_from_this<FontCollection> {
 public:
  FontCollection();

  ~FontCollection();

  size_t GetFontManagersCount() const;

  void SetupDefaultFontManager(uint32_t font_initialization_data);
  void SetDefaultFontManager(sk_sp<SkFontMgr> font_manager);
  void SetAssetFontManager(sk_sp<SkFontMgr> font_manager);
  void SetDynamicFontManager(sk_sp<SkFontMgr> font_manager);
  void SetTestFontManager(sk_sp<SkFontMgr> font_manager);

  // Do not provide alternative fonts that can match characters which are
  // missing from the requested font family.
  void DisableFontFallback();

  // Remove all entries in the font family cache.
  void ClearFontFamilyCache();

  // Construct a Skia text layout FontCollection based on this collection.
  sk_sp<skia::textlayout::FontCollection> CreateSktFontCollection();

 private:
  sk_sp<SkFontMgr> default_font_manager_;
  sk_sp<SkFontMgr> asset_font_manager_;
  sk_sp<SkFontMgr> dynamic_font_manager_;
  sk_sp<SkFontMgr> test_font_manager_;
  bool enable_font_fallback_;

  // An equivalent font collection usable by the Skia text shaper library.
  sk_sp<skia::textlayout::FontCollection> skt_collection_;

  std::vector<sk_sp<SkFontMgr>> GetFontManagerOrder() const;

  FML_DISALLOW_COPY_AND_ASSIGN(FontCollection);
};

}  // namespace txt

#endif  // APPCODE_TXT_SRC_TXT_FONT_COLLECTION_H_
