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

#include "impeller/toolkit/interop/backend/gles/reactor_worker_gles.h"

namespace impeller::interop {

ReactorWorkerGLES::ReactorWorkerGLES()
    : thread_id_(std::this_thread::get_id()) {}

ReactorWorkerGLES::~ReactorWorkerGLES() = default;

bool ReactorWorkerGLES::CanReactorReactOnCurrentThreadNow(
    const ReactorGLES& reactor) const {
  return thread_id_ == std::this_thread::get_id();
}

}  // namespace impeller::interop
