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

#include "fuchsia_intl.h"

#include <sstream>
#include <string>
#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "runner.h"
#include "runtime/dart/utils/tempfs.h"
#include "third_party/icu/source/common/unicode/bytestream.h"
#include "third_party/icu/source/common/unicode/errorcode.h"
#include "third_party/icu/source/common/unicode/locid.h"
#include "third_party/icu/source/common/unicode/strenum.h"
#include "third_party/icu/source/common/unicode/stringpiece.h"
#include "third_party/icu/source/common/unicode/uloc.h"

using icu::Locale;

namespace appcode_runner {

using fuchsia::intl::Profile;

fml::MallocMapping MakeLocalizationPlatformMessageData(
    const Profile& intl_profile) {
  rapidjson::Document document;
  auto& allocator = document.GetAllocator();
  document.SetObject();
  document.AddMember("method", "setLocale", allocator);
  rapidjson::Value args(rapidjson::kArrayType);

  for (const auto& locale_id : intl_profile.locales()) {
    UErrorCode error_code = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(locale_id.id, error_code);
    if (U_FAILURE(error_code)) {
      FML_LOG(ERROR) << "Error parsing locale ID \"" << locale_id.id << "\"";
      continue;
    }
    args.PushBack(rapidjson::Value().SetString(locale.getLanguage(), allocator),
                  allocator);

    auto country = locale.getCountry() != nullptr ? locale.getCountry() : "";
    args.PushBack(rapidjson::Value().SetString(country, allocator), allocator);

    auto script = locale.getScript() != nullptr ? locale.getScript() : "";
    args.PushBack(rapidjson::Value().SetString(script, allocator), allocator);

    std::string variant =
        locale.getVariant() != nullptr ? locale.getVariant() : "";
    // ICU4C capitalizes the variant for backward compatibility, even though
    // the preferred form is lowercase.  So we lowercase here.
    std::transform(begin(variant), end(variant), begin(variant),
                   [](unsigned char c) { return std::tolower(c); });
    args.PushBack(rapidjson::Value().SetString(variant, allocator), allocator);
  }

  document.AddMember("args", args, allocator);

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  document.Accept(writer);
  auto data = reinterpret_cast<const uint8_t*>(buffer.GetString());
  return fml::MallocMapping::Copy(data, buffer.GetSize());
}

}  // namespace appcode_runner
