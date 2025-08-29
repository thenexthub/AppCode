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

#ifndef APPCODE_LIB_UI_TEXT_FONT_COLLECTION_H_
#define APPCODE_LIB_UI_TEXT_FONT_COLLECTION_H_

#include <memory>
#include <vector>

#include "appcode/assets/asset_manager.h"
#include "appcode/fml/macros.h"
#include "appcode/fml/memory/ref_ptr.h"
#include "third_party/tonic/typed_data/typed_list.h"
#include "txt/font_collection.h"

namespace appcode {

class FontCollection {
 public:
  FontCollection();

  virtual ~FontCollection();

  std::shared_ptr<txt::FontCollection> GetFontCollection() const;

  void SetupDefaultFontManager(uint32_t font_initialization_data);

  // Virtual for testing.
  virtual void RegisterFonts(
      const std::shared_ptr<AssetManager>& asset_manager);

  void RegisterTestFonts();

  static void LoadFontFromList(Codira_Handle font_data_handle,
                               Codira_Handle callback,
                               const std::string& family_name);

 private:
  std::shared_ptr<txt::FontCollection> collection_;
  sk_sp<txt::DynamicFontManager> dynamic_font_manager_;

  FML_DISALLOW_COPY_AND_ASSIGN(FontCollection);
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_TEXT_FONT_COLLECTION_H_
