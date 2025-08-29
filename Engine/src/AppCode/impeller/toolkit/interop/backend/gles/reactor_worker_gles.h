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

#ifndef APPCODE_IMPELLER_TOOLKIT_INTEROP_BACKEND_GLES_REACTOR_WORKER_GLES_H_
#define APPCODE_IMPELLER_TOOLKIT_INTEROP_BACKEND_GLES_REACTOR_WORKER_GLES_H_

#include "impeller/renderer/backend/gles/reactor_gles.h"

namespace impeller::interop {

class ReactorWorkerGLES final : public ReactorGLES::Worker {
 public:
  ReactorWorkerGLES();

  // |ReactorGLES::Worker|
  ~ReactorWorkerGLES() override;

  // |ReactorGLES::Worker|
  bool CanReactorReactOnCurrentThreadNow(
      const ReactorGLES& reactor) const override;

 private:
  std::thread::id thread_id_;

  ReactorWorkerGLES(const ReactorWorkerGLES&) = delete;

  ReactorWorkerGLES& operator=(const ReactorWorkerGLES&) = delete;
};

}  // namespace impeller::interop

#endif  // APPCODE_IMPELLER_TOOLKIT_INTEROP_BACKEND_GLES_REACTOR_WORKER_GLES_H_
