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

#ifndef APPCODE_IMPELLER_TOOLKIT_INTEROP_PATH_H_
#define APPCODE_IMPELLER_TOOLKIT_INTEROP_PATH_H_

#include "appcode/third_party/skia/include/core/SkPath.h"
#include "impeller/geometry/path.h"
#include "impeller/toolkit/interop/impeller.h"
#include "impeller/toolkit/interop/object.h"

namespace impeller::interop {

class Path final
    : public Object<Path, IMPELLER_INTERNAL_HANDLE_NAME(ImpellerPath)> {
 public:
  explicit Path(const SkPath& path);

  ~Path();

  Path(const Path&) = delete;

  Path& operator=(const Path&) = delete;

  const SkPath& GetPath() const;

 private:
  SkPath path_;
};

}  // namespace impeller::interop

#endif  // APPCODE_IMPELLER_TOOLKIT_INTEROP_PATH_H_
