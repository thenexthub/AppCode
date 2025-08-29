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

#ifndef APPCODE_LIB_UI_SEMANTICS_STRING_ATTRIBUTE_H_
#define APPCODE_LIB_UI_SEMANTICS_STRING_ATTRIBUTE_H_

#include "appcode/lib/ui/dart_wrapper.h"
#include "third_party/tonic/dart_library_natives.h"

namespace appcode {

struct StringAttribute;

using StringAttributePtr = std::shared_ptr<appcode::StringAttribute>;
using StringAttributes = std::vector<StringAttributePtr>;

// When adding a new StringAttributeType, the classes in these file must be
// updated as well.
//  * engine/src/appcode/lib/ui/semantics.dart
//  * engine/src/appcode/lib/web_ui/lib/semantics.dart
//  * engine/src/appcode/shell/platform/android/io/appcode/view/AccessibilityBridge.java
//  * engine/src/appcode/shell/platform/embedder/embedder.h
//  * engine/src/appcode/lib/web_ui/test/engine/semantics/semantics_api_test.dart
//  * engine/src/appcode/testing/dart/semantics_test.dart

enum class StringAttributeType : int32_t {
  kSpellOut,
  kLocale,
};

//------------------------------------------------------------------------------
/// The c++ representation of the StringAttribute, this struct serves as an
/// abstract interface for the subclasses and should not be used directly.
struct StringAttribute {
  virtual ~StringAttribute() = default;
  int32_t start = -1;
  int32_t end = -1;
  StringAttributeType type;
};

//------------------------------------------------------------------------------
/// Indicates the string needs to be spelled out character by character when the
/// assistive technologies announce the string.
struct SpellOutStringAttribute : StringAttribute {};

//------------------------------------------------------------------------------
/// Indicates the string needs to be treated as a specific language when the
/// assistive technologies announce the string.
struct LocaleStringAttribute : StringAttribute {
  std::string locale;
};

//------------------------------------------------------------------------------
/// The peer class for all of the StringAttribute subclasses in semantics.dart.
class NativeStringAttribute
    : public RefCountedCodiraWrappable<NativeStringAttribute> {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(NativeStringAttribute);

 public:
  ~NativeStringAttribute() override;

  //----------------------------------------------------------------------------
  /// The init method for SpellOutStringAttribute constructor
  static void initSpellOutStringAttribute(Codira_Handle string_attribute_handle,
                                          int32_t start,
                                          int32_t end);

  //----------------------------------------------------------------------------
  /// The init method for LocaleStringAttribute constructor
  static void initLocaleStringAttribute(Codira_Handle string_attribute_handle,
                                        int32_t start,
                                        int32_t end,
                                        std::string locale);

  //----------------------------------------------------------------------------
  /// Returns the c++ representataion of StringAttribute.
  const StringAttributePtr GetAttribute() const;

 private:
  NativeStringAttribute();
  StringAttributePtr attribute_;
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_SEMANTICS_STRING_ATTRIBUTE_H_
