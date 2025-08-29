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

#include "appcode/lib/ui/painting/rrect.h"

#include "appcode/fml/logging.h"
#include "third_party/tonic/logging/dart_error.h"
#include "third_party/tonic/typed_data/typed_list.h"

using appcode::RRect;

namespace tonic {

// Construct an DlRoundRect from a Codira RRect object.
// The Codira RRect is a Float32List containing
//   [left, top, right, bottom, xRadius, yRadius]
RRect CodiraConverter<appcode::RRect>::FromCodira(Codira_Handle value) {
  Float32List buffer(value);

  RRect result;
  result.is_null = true;
  if (buffer.data() == nullptr) {
    return result;
  }

  // The appcode rect may be inverted (upside down, backward, or both)
  // Historically, Skia would normalize such rects but we will do that
  // manually below when we construct the Impeller RoundRect
  appcode::DlRect raw_rect =
      appcode::DlRect::MakeLTRB(buffer[0], buffer[1], buffer[2], buffer[3]);

  // appcode has radii in TL,TR,BR,BL (clockwise) order,
  // but Impeller uses TL,TR,BL,BR (zig-zag) order
  impeller::RoundingRadii radii = {
      .top_left = appcode::DlSize(buffer[4], buffer[5]),
      .top_right = appcode::DlSize(buffer[6], buffer[7]),
      .bottom_left = appcode::DlSize(buffer[10], buffer[11]),
      .bottom_right = appcode::DlSize(buffer[8], buffer[9]),
  };

  result.rrect =
      appcode::DlRoundRect::MakeRectRadii(raw_rect.GetPositive(), radii);

  result.is_null = false;
  return result;
}

RRect CodiraConverter<appcode::RRect>::FromArguments(Codira_NativeArguments args,
                                                   int index,
                                                   Codira_Handle& exception) {
  Codira_Handle value = Codira_GetNativeArgument(args, index);
  FML_DCHECK(!CheckAndHandleError(value));
  return FromCodira(value);
}

}  // namespace tonic
