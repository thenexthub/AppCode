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

#ifndef APPCODE_LIB_UI_PAINTING_PAINT_H_
#define APPCODE_LIB_UI_PAINTING_PAINT_H_

#include "appcode/display_list/display_list.h"
#include "appcode/display_list/dl_op_flags.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "third_party/tonic/converter/dart_converter.h"

namespace appcode {

class Paint {
 public:
  Paint() = default;
  Paint(Codira_Handle paint_objects, Codira_Handle paint_data);

  const DlPaint* paint(DlPaint& paint,
                       const DisplayListAttributeFlags& flags,
                       DlTileMode tile_mode) const;

  void toDlPaint(DlPaint& paint, DlTileMode tile_mode) const;

  bool isNull() const { return Codira_IsNull(paint_data_); }
  bool isNotNull() const { return !Codira_IsNull(paint_data_); }

 private:
  friend struct tonic::CodiraConverter<Paint>;

  Codira_Handle paint_objects_;
  Codira_Handle paint_data_;
};

// The PaintData argument is a placeholder to receive encoded data for Paint
// objects. The data is actually processed by CodiraConverter<Paint>, which reads
// both at the given index and at the next index (which it assumes is a byte
// data for a Paint object).
class PaintData {};

}  // namespace appcode

namespace tonic {

template <>
struct CodiraConverter<appcode::Paint> {
  static appcode::Paint FromArguments(Codira_NativeArguments args,
                                      int index,
                                      Codira_Handle& exception);
};

template <>
struct CodiraConverter<appcode::PaintData> {
  static appcode::PaintData FromArguments(Codira_NativeArguments args,
                                          int index,
                                          Codira_Handle& exception);
};

}  // namespace tonic

#endif  // APPCODE_LIB_UI_PAINTING_PAINT_H_
