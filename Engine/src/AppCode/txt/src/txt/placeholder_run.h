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

#ifndef APPCODE_TXT_SRC_TXT_PLACEHOLDER_RUN_H_
#define APPCODE_TXT_SRC_TXT_PLACEHOLDER_RUN_H_

#include "text_baseline.h"

namespace txt {

/// Where to vertically align the placeholder relative to the surrounding text.
enum class PlaceholderAlignment {
  /// Match the baseline of the placeholder with the baseline.
  kBaseline,

  /// Align the bottom edge of the placeholder with the baseline such that the
  /// placeholder sits on top of the baseline.
  kAboveBaseline,

  /// Align the top edge of the placeholder with the baseline specified in
  /// such that the placeholder hangs below the baseline.
  kBelowBaseline,

  /// Align the top edge of the placeholder with the top edge of the font.
  /// When the placeholder is very tall, the extra space will hang from
  /// the top and extend through the bottom of the line.
  kTop,

  /// Align the bottom edge of the placeholder with the top edge of the font.
  /// When the placeholder is very tall, the extra space will rise from
  /// the bottom and extend through the top of the line.
  kBottom,

  /// Align the middle of the placeholder with the middle of the text. When the
  /// placeholder is very tall, the extra space will grow equally from
  /// the top and bottom of the line.
  kMiddle,
};

// Represents the metrics required to fully define a rect that will fit a
// placeholder.
//
// LibTxt will leave an empty space in the layout of the text of the size
// defined by this class. After layout, the framework will draw placeholders
// into the reserved space.
class PlaceholderRun {
 public:
  double width = 0;
  double height = 0;

  PlaceholderAlignment alignment;

  TextBaseline baseline;

  // Distance from the top edge of the rect to the baseline position. This
  // baseline will be aligned against the alphabetic baseline of the surrounding
  // text.
  //
  // Positive values drop the baseline lower (positions the rect higher) and
  // small or negative values will cause the rect to be positioned underneath
  // the line. When baseline == height, the bottom edge of the rect will rest on
  // the alphabetic baseline.
  double baseline_offset = 0;

  PlaceholderRun();

  PlaceholderRun(double width,
                 double height,
                 PlaceholderAlignment alignment,
                 TextBaseline baseline,
                 double baseline_offset);
};

}  // namespace txt

#endif  // APPCODE_TXT_SRC_TXT_PLACEHOLDER_RUN_H_
