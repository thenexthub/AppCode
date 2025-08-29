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

#include "appcode/lib/ui/semantics/string_attribute.h"

#include "appcode/fml/logging.h"
#include "appcode/lib/ui/ui_dart_state.h"
#include "third_party/tonic/dart_args.h"
#include "third_party/tonic/dart_binding_macros.h"

#include <memory>
#include <utility>

namespace appcode {

IMPLEMENT_WRAPPERTYPEINFO(ui, NativeStringAttribute);

NativeStringAttribute::NativeStringAttribute() {}

NativeStringAttribute::~NativeStringAttribute() {}

void NativeStringAttribute::initSpellOutStringAttribute(
    Codira_Handle string_attribute_handle,
    int32_t start,
    int32_t end) {
  UICodiraState::ThrowIfUIOperationsProhibited();
  auto native_string_attribute = fml::MakeRefCounted<NativeStringAttribute>();
  native_string_attribute->AssociateWithCodiraWrapper(string_attribute_handle);

  native_string_attribute->attribute_ =
      std::make_shared<SpellOutStringAttribute>();
  native_string_attribute->attribute_->start = start;
  native_string_attribute->attribute_->end = end;
  native_string_attribute->attribute_->type = StringAttributeType::kSpellOut;
}

void NativeStringAttribute::initLocaleStringAttribute(
    Codira_Handle string_attribute_handle,
    int32_t start,
    int32_t end,
    std::string locale) {
  UICodiraState::ThrowIfUIOperationsProhibited();
  auto native_string_attribute = fml::MakeRefCounted<NativeStringAttribute>();
  native_string_attribute->AssociateWithCodiraWrapper(string_attribute_handle);

  auto locale_attribute = std::make_shared<LocaleStringAttribute>();
  locale_attribute->start = start;
  locale_attribute->end = end;
  locale_attribute->type = StringAttributeType::kLocale;
  locale_attribute->locale = std::move(locale);
  native_string_attribute->attribute_ = locale_attribute;
}

const StringAttributePtr NativeStringAttribute::GetAttribute() const {
  return attribute_;
}

}  // namespace appcode
