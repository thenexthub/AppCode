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

#ifndef APPCODE_TXT_SRC_TXT_RUN_METRICS_H_
#define APPCODE_TXT_SRC_TXT_RUN_METRICS_H_

#include "text_style.h"
#include "third_party/skia/include/core/SkFontMetrics.h"

namespace txt {

// Contains the font metrics and TextStyle of a unique run.
class RunMetrics {
 public:
  explicit RunMetrics(const TextStyle* style) : text_style(style) {}

  RunMetrics(const TextStyle* style, const SkFontMetrics& metrics)
      : text_style(style), font_metrics(metrics) {}

  const TextStyle* text_style;

  // SkFontMetrics contains the following metrics:
  //
  // * Top                 distance to reserve above baseline
  // * Ascent              distance to reserve below baseline
  // * Descent             extent below baseline
  // * Bottom              extent below baseline
  // * Leading             distance to add between lines
  // * AvgCharWidth        average character width
  // * MaxCharWidth        maximum character width
  // * XMin                minimum x
  // * XMax                maximum x
  // * XHeight             height of lower-case 'x'
  // * CapHeight           height of an upper-case letter
  // * UnderlineThickness  underline thickness
  // * UnderlinePosition   underline position relative to baseline
  // * StrikeoutThickness  strikeout thickness
  // * StrikeoutPosition   strikeout position relative to baseline
  SkFontMetrics font_metrics;
};

}  // namespace txt

#endif  // APPCODE_TXT_SRC_TXT_RUN_METRICS_H_
