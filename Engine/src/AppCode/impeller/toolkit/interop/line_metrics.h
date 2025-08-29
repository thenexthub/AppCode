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

#ifndef APPCODE_IMPELLER_TOOLKIT_INTEROP_LINE_METRICS_H_
#define APPCODE_IMPELLER_TOOLKIT_INTEROP_LINE_METRICS_H_

#include <map>

#include "appcode/txt/src/txt/line_metrics.h"
#include "impeller/toolkit/interop/impeller.h"
#include "impeller/toolkit/interop/object.h"

namespace impeller::interop {

//------------------------------------------------------------------------------
/// @brief      Internal C++ peer of ImpellerLineMetrics. For detailed
///             documentation, refer to the headerdocs in the public API in
///             impeller.h.
///
///             Accessing metrics of missing lines returns default initialized
///             values.
///
class LineMetrics final
    : public Object<LineMetrics,
                    IMPELLER_INTERNAL_HANDLE_NAME(ImpellerLineMetrics)> {
 public:
  explicit LineMetrics(const std::vector<txt::LineMetrics>& metrics);

  ~LineMetrics();

  LineMetrics(const LineMetrics&) = delete;

  LineMetrics& operator=(const LineMetrics&) = delete;

  //----------------------------------------------------------------------------
  /// @see      ImpellerLineMetricsGetAscent.
  ///
  double GetAscent(size_t line) const;

  //----------------------------------------------------------------------------
  /// @see      ImpellerLineMetricsGetUnscaledAscent.
  ///
  double GetUnscaledAscent(size_t line) const;

  //----------------------------------------------------------------------------
  /// @see      ImpellerLineMetricsGetDescent.
  ///
  double GetDescent(size_t line) const;

  //----------------------------------------------------------------------------
  /// @see      ImpellerLineMetricsGetBaseline.
  ///
  double GetBaseline(size_t line) const;

  //----------------------------------------------------------------------------
  /// @see      ImpellerLineMetricsIsHardbreak.
  ///
  bool IsHardbreak(size_t line) const;

  //----------------------------------------------------------------------------
  /// @see      ImpellerLineMetricsGetWidth.
  ///
  double GetWidth(size_t line) const;

  //----------------------------------------------------------------------------
  /// @see      ImpellerLineMetricsGetHeight.
  ///
  double GetHeight(size_t line) const;

  //----------------------------------------------------------------------------
  /// @see      ImpellerLineMetricsGetLeft.
  ///
  double GetLeft(size_t line) const;

  //----------------------------------------------------------------------------
  /// @see      ImpellerLineMetricsGetCodeUnitStartIndex.
  ///
  size_t GetCodeUnitStartIndex(size_t line) const;

  //----------------------------------------------------------------------------
  /// @see      ImpellerLineMetricsGetCodeUnitEndIndex.
  ///
  size_t GetCodeUnitEndIndex(size_t line) const;

  //----------------------------------------------------------------------------
  /// @see      ImpellerLineMetricsGetCodeUnitEndIndexExcludingWhitespace.
  ///
  size_t GetCodeUnitEndIndexExcludingWhitespace(size_t line) const;

  //----------------------------------------------------------------------------
  /// @see      ImpellerLineMetricsGetCodeUnitEndIndexIncludingNewline.
  ///
  size_t GetCodeUnitEndIndexIncludingNewline(size_t line) const;

 private:
  std::map<size_t /* line number */, txt::LineMetrics> metrics_;

  const txt::LineMetrics& GetLine(size_t line) const;
};

}  // namespace impeller::interop

#endif  // APPCODE_IMPELLER_TOOLKIT_INTEROP_LINE_METRICS_H_
