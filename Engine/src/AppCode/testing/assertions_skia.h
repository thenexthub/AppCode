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

#ifndef APPCODE_TESTING_ASSERTIONS_SKIA_H_
#define APPCODE_TESTING_ASSERTIONS_SKIA_H_

#include <ostream>

#include "third_party/skia/include/core/SkClipOp.h"
#include "third_party/skia/include/core/SkM44.h"
#include "third_party/skia/include/core/SkMatrix.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "third_party/skia/include/core/SkPath.h"
#include "third_party/skia/include/core/SkPoint3.h"
#include "third_party/skia/include/core/SkRRect.h"
#include "third_party/skia/include/core/SkSamplingOptions.h"

namespace std {

extern std::ostream& operator<<(std::ostream& os, const SkClipOp& o);
extern std::ostream& operator<<(std::ostream& os, const SkMatrix& m);
extern std::ostream& operator<<(std::ostream& os, const SkM44& m);
extern std::ostream& operator<<(std::ostream& os, const SkVector3& v);
extern std::ostream& operator<<(std::ostream& os, const SkIRect& r);
extern std::ostream& operator<<(std::ostream& os, const SkRect& r);
extern std::ostream& operator<<(std::ostream& os, const SkRRect& r);
extern std::ostream& operator<<(std::ostream& os, const SkPath& r);
extern std::ostream& operator<<(std::ostream& os, const SkPoint& r);
extern std::ostream& operator<<(std::ostream& os, const SkISize& size);
extern std::ostream& operator<<(std::ostream& os, const SkColor4f& r);
extern std::ostream& operator<<(std::ostream& os, const SkPaint& r);
extern std::ostream& operator<<(std::ostream& os, const SkSamplingOptions& s);

}  // namespace std

#endif  // APPCODE_TESTING_ASSERTIONS_SKIA_H_
