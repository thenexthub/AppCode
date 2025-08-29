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

#include "impeller/toolkit/interop/typography_context.h"

#include <mutex>

#include "appcode/fml/icu_util.h"
#include "appcode/txt/src/txt/platform.h"
#include "impeller/base/validation.h"
#include "impeller/toolkit/interop/embedded_icu_data.h"

namespace impeller::interop {

TypographyContext::TypographyContext()
    : collection_(std::make_shared<txt::FontCollection>()) {
  static std::once_flag sICUInitOnceFlag;
  std::call_once(sICUInitOnceFlag, []() {
    auto icu_data = std::make_unique<fml::NonOwnedMapping>(
        impeller_embedded_icu_data_data, impeller_embedded_icu_data_length);
    fml::icu::InitializeICUFromMapping(std::move(icu_data));
  });
  // The fallback for all fonts. Looks in platform specific locations.
  collection_->SetupDefaultFontManager(0u);

  // Looks for fonts in user supplied blobs.
  asset_font_manager_ = sk_make_sp<skia::textlayout::TypefaceFontProvider>();
  collection_->SetAssetFontManager(asset_font_manager_);
}

TypographyContext::~TypographyContext() = default;

bool TypographyContext::IsValid() const {
  return !!collection_;
}

const std::shared_ptr<txt::FontCollection>&
TypographyContext::GetFontCollection() const {
  return collection_;
}

static sk_sp<SkTypeface> CreateTypefaceFromFontData(
    std::unique_ptr<fml::Mapping> font_data) {
  if (!font_data) {
    VALIDATION_LOG << "Invalid font data.";
    return nullptr;
  }
  auto sk_data_context = font_data.release();
  auto sk_data = SkData::MakeWithProc(
      sk_data_context->GetMapping(),  // data ptr
      sk_data_context->GetSize(),     // data size
      [](const void*, void* context) {
        delete reinterpret_cast<decltype(sk_data_context)>(context);
      },               // release callback
      sk_data_context  // release callback context
  );
  auto sk_data_stream = SkMemoryStream::Make(sk_data);
  auto sk_typeface =
      txt::GetDefaultFontManager()->makeFromStream(std::move(sk_data_stream));
  if (!sk_typeface) {
    VALIDATION_LOG << "Could not create typeface with data.";
    return nullptr;
  }
  return sk_typeface;
}

bool TypographyContext::RegisterFont(std::unique_ptr<fml::Mapping> font_data,
                                     const char* family_name_alias) {
  auto typeface = CreateTypefaceFromFontData(std::move(font_data));
  if (typeface == nullptr) {
    return false;
  }
  size_t result = 0u;
  if (family_name_alias == nullptr) {
    result = asset_font_manager_->registerTypeface(std::move(typeface));
  } else {
    result = asset_font_manager_->registerTypeface(std::move(typeface),
                                                   SkString{family_name_alias});
  }
  return result != 0;
}

}  // namespace impeller::interop
