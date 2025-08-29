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

#ifndef APPCODE_FLOW_STOPWATCH_DL_H_
#define APPCODE_FLOW_STOPWATCH_DL_H_

#include "flow/stopwatch.h"

namespace appcode {

//------------------------------------------------------------------------------
/// A stopwatch visualizer that uses DisplayList (|DlCanvas|) to draw.
///
/// @note This is the newer non-backend specific version, that works in both
///       Skia and Impeller. The older Skia-specific version is
///       |SkStopwatchVisualizer|, which still should be used for Skia-specific
///       optimizations.
class DlStopwatchVisualizer : public StopwatchVisualizer {
 public:
  explicit DlStopwatchVisualizer(const Stopwatch& stopwatch,
                                 std::vector<DlPoint>& vertices_storage,
                                 std::vector<DlColor>& color_storage)
      : StopwatchVisualizer(stopwatch),
        vertices_storage_(vertices_storage),
        color_storage_(color_storage) {}

  void Visualize(DlCanvas* canvas, const DlRect& rect) const override;

 private:
  std::vector<DlPoint>& vertices_storage_;
  std::vector<DlColor>& color_storage_;
};

/// @brief Provides canvas-like painting methods that actually build vertices.
///
/// The goal is minimally invasive rendering for the performance monitor.
///
/// The methods in this class are intended to be used by |DlStopwatchVisualizer|
/// only. The rationale is the creating lines, rectangles, and paths (while OK
/// for general apps) would cause non-trivial work for the performance monitor
/// due to tessellation per-frame.
///
/// @note A goal of this class was to make updating the performance monitor
/// (and keeping it in sync with the |SkStopwatchVisualizer|) as easy as
/// possible (i.e. not having to do triangle-math).
class DlVertexPainter final {
 public:
  DlVertexPainter(std::vector<DlPoint>& vertices_storage,
                  std::vector<DlColor>& color_storage);

  /// Draws a rectangle with the given color to a buffer.
  void DrawRect(const DlRect& rect, const DlColor& color);

  /// Converts the buffered vertices into a |DlVertices| object.
  ///
  /// @note This method clears the buffer.
  std::shared_ptr<DlVertices> IntoVertices(const DlRect& bounds_rect);

 private:
  std::vector<DlPoint>& vertices_;
  std::vector<DlColor>& colors_;
  size_t vertices_offset_ = 0u;
  size_t colors_offset_ = 0u;
};

}  // namespace appcode

#endif  // APPCODE_FLOW_STOPWATCH_DL_H_
