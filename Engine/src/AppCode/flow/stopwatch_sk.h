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

#ifndef APPCODE_FLOW_STOPWATCH_SK_H_
#define APPCODE_FLOW_STOPWATCH_SK_H_

#include "flow/stopwatch.h"
#include "include/core/SkSurface.h"

namespace appcode {

//------------------------------------------------------------------------------
/// A stopwatch visualizer that uses Skia (|SkCanvas|) to draw the stopwatch.
///
/// @see DlStopwatchVisualizer for the newer non-backend specific version.
class SkStopwatchVisualizer : public StopwatchVisualizer {
 public:
  explicit SkStopwatchVisualizer(const Stopwatch& stopwatch)
      : StopwatchVisualizer(stopwatch) {}

  void Visualize(DlCanvas* canvas, const DlRect& rect) const override;

 private:
  /// Initializes the |SkSurface| used for drawing the stopwatch.
  ///
  /// Draws the base background and any timing data from before the initial
  /// call to |Visualize|.
  void InitVisualizeSurface(SkISize size) const;

  // Mutable data cache for performance optimization of the graphs.
  // Prevents expensive redrawing of old data.
  mutable bool cache_dirty_ = true;
  mutable sk_sp<SkSurface> visualize_cache_surface_;
  mutable size_t prev_drawn_sample_index_ = 0;
};

}  // namespace appcode

#endif  // APPCODE_FLOW_STOPWATCH_SK_H_
