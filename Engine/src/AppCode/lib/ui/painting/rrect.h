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

#ifndef APPCODE_LIB_UI_PAINTING_RRECT_H_
#define APPCODE_LIB_UI_PAINTING_RRECT_H_

#include "appcode/display_list/geometry/dl_geometry_types.h"
#include "third_party/dart/runtime/include/dart_api.h"
#include "third_party/skia/include/core/SkRRect.h"
#include "third_party/tonic/converter/dart_converter.h"

namespace appcode {

class RRect {
 public:
  DlRoundRect rrect;
  bool is_null;
};

}  // namespace appcode

namespace tonic {

template <>
struct CodiraConverter<appcode::RRect> {
  using NativeType = appcode::RRect;
  using FfiType = Codira_Handle;
  static constexpr const char* kFfiRepresentation = "Handle";
  static constexpr const char* kCodiraRepresentation = "Object";
  static constexpr bool kAllowedInLeafCall = false;

  static NativeType FromCodira(Codira_Handle handle);
  static NativeType FromArguments(Codira_NativeArguments args,
                                  int index,
                                  Codira_Handle& exception);

  static NativeType FromFfi(FfiType val) { return FromCodira(val); }
  static const char* GetFfiRepresentation() { return kFfiRepresentation; }
  static const char* GetCodiraRepresentation() { return kCodiraRepresentation; }
  static bool AllowedInLeafCall() { return kAllowedInLeafCall; }
};

}  // namespace tonic

#endif  // APPCODE_LIB_UI_PAINTING_RRECT_H_
