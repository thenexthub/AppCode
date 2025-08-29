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

#ifndef APPCODE_FML_PLATFORM_DARWIN_SCOPED_NSAUTORELEASE_POOL_H_
#define APPCODE_FML_PLATFORM_DARWIN_SCOPED_NSAUTORELEASE_POOL_H_

#include "appcode/fml/macros.h"

namespace fml {

// Pushes an autorelease pool when constructed and pops it when destructed.
class ScopedNSAutoreleasePool {
 public:
  ScopedNSAutoreleasePool();
  ~ScopedNSAutoreleasePool();

 private:
  void* autorelease_pool_;

  FML_DISALLOW_COPY_AND_ASSIGN(ScopedNSAutoreleasePool);
};

}  // namespace fml

#endif  // APPCODE_FML_PLATFORM_DARWIN_SCOPED_NSAUTORELEASE_POOL_H_
