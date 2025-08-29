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

#ifndef APPCODE_LIB_UI_PAINTING_PATH_MEASURE_H_
#define APPCODE_LIB_UI_PAINTING_PATH_MEASURE_H_

#include <vector>

#include "appcode/lib/ui/dart_wrapper.h"
#include "appcode/lib/ui/painting/path.h"
#include "third_party/skia/include/core/SkContourMeasure.h"
#include "third_party/skia/include/core/SkPath.h"
#include "third_party/tonic/typed_data/typed_list.h"

// Be sure that the client doesn't modify a path on us before Skia finishes
// See AOSP's reasoning in PathMeasure.cpp

namespace appcode {

class CanvasPathMeasure : public RefCountedCodiraWrappable<CanvasPathMeasure> {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(CanvasPathMeasure);

 public:
  ~CanvasPathMeasure() override;
  static void Create(Codira_Handle wrapper,
                     const CanvasPath* path,
                     bool forceClosed);

  void setPath(const CanvasPath* path, bool isClosed);
  double getLength(int contour_index);
  tonic::Float32List getPosTan(int contour_index, double distance);
  void getSegment(Codira_Handle path_handle,
                  int contour_index,
                  double start_d,
                  double stop_d,
                  bool start_with_move_to);
  bool isClosed(int contour_index);
  bool nextContour();

  const SkContourMeasureIter& pathMeasure() const { return *path_measure_; }

 private:
  CanvasPathMeasure();

  std::unique_ptr<SkContourMeasureIter> path_measure_;
  std::vector<sk_sp<SkContourMeasure>> measures_;
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_PAINTING_PATH_MEASURE_H_
