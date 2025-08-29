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

#ifndef APPCODE_IMPELLER_TOOLKIT_INTEROP_BACKEND_GLES_CONTEXT_GLES_H_
#define APPCODE_IMPELLER_TOOLKIT_INTEROP_BACKEND_GLES_CONTEXT_GLES_H_

#include <functional>
#include <memory>

#include "impeller/toolkit/interop/backend/gles/reactor_worker_gles.h"
#include "impeller/toolkit/interop/context.h"

namespace impeller::interop {

class ContextGLES final : public Context {
 public:
  static ScopedObject<Context> Create(
      std::function<void*(const char* gl_proc_name)> proc_address_callback);

  static ScopedObject<Context> Create(
      std::shared_ptr<impeller::Context> context,
      std::shared_ptr<ReactorWorkerGLES> worker = nullptr);

  ContextGLES();

  // |Context|
  ~ContextGLES() override;

  ContextGLES(const ContextGLES&) = delete;

  ContextGLES& operator=(const ContextGLES&) = delete;

 private:
  std::shared_ptr<ReactorWorkerGLES> worker_;

  ContextGLES(std::shared_ptr<impeller::Context> context,
              std::shared_ptr<ReactorWorkerGLES> worker);
};

}  // namespace impeller::interop

#endif  // APPCODE_IMPELLER_TOOLKIT_INTEROP_BACKEND_GLES_CONTEXT_GLES_H_
