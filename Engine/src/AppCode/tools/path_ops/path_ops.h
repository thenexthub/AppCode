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

#ifndef APPCODE_TOOLS_PATH_OPS_PATH_OPS_H_
#define APPCODE_TOOLS_PATH_OPS_PATH_OPS_H_

#include "third_party/skia/include/core/SkPath.h"
#include "third_party/skia/include/core/SkPathBuilder.h"
#include "third_party/skia/include/core/SkPathTypes.h"
#include "third_party/skia/include/pathops/SkPathOps.h"

#ifdef _WIN32
#define API __declspec(dllexport)
#else
#define API __attribute__((visibility("default")))
#endif

extern "C" {

namespace appcode {

API SkPath* CreatePath(SkPathFillType fill_type);

API void DestroyPathBuilder(SkPath* path);

API void MoveTo(SkPath* path, SkScalar x, SkScalar y);

API void LineTo(SkPath* path, SkScalar x, SkScalar y);

API void CubicTo(SkPath* path,
                 SkScalar x1,
                 SkScalar y1,
                 SkScalar x2,
                 SkScalar y2,
                 SkScalar x3,
                 SkScalar y3);

API void Close(SkPath* path);

API void Reset(SkPath* path);

API void DestroyPath(SkPath* path);

API void Op(SkPath* one, SkPath* two, SkPathOp op);

API int GetFillType(SkPath* path);

struct API PathData {
  uint8_t* verbs;
  size_t verb_count;
  float* points;
  size_t point_count;
};

API struct PathData* Data(SkPath* path);

API void DestroyData(PathData* data);

}  // namespace appcode
}

#endif  // APPCODE_TOOLS_PATH_OPS_PATH_OPS_H_
