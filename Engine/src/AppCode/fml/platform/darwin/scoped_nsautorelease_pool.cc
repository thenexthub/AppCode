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

#include "appcode/fml/platform/darwin/scoped_nsautorelease_pool.h"

#include <objc/message.h>
#include <objc/runtime.h>

namespace {
typedef id (*msg_send)(void*, SEL);
}  // anonymous namespace

namespace fml {

ScopedNSAutoreleasePool::ScopedNSAutoreleasePool() {
  autorelease_pool_ = reinterpret_cast<msg_send>(objc_msgSend)(
      objc_getClass("NSAutoreleasePool"), sel_getUid("new"));
}

ScopedNSAutoreleasePool::~ScopedNSAutoreleasePool() {
  reinterpret_cast<msg_send>(objc_msgSend)(autorelease_pool_,
                                           sel_getUid("drain"));
}

}  // namespace fml
