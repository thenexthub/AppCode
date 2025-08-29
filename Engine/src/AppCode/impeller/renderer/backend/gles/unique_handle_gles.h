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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_GLES_UNIQUE_HANDLE_GLES_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_GLES_UNIQUE_HANDLE_GLES_H_

#include "impeller/renderer/backend/gles/handle_gles.h"
#include "impeller/renderer/backend/gles/reactor_gles.h"

namespace impeller {

//------------------------------------------------------------------------------
/// @brief      A unique handle to an OpenGL object. The collection of this
///             handle scheduled the destruction of the associated OpenGL object
///             in the reactor.
///
class UniqueHandleGLES {
 public:
  UniqueHandleGLES(std::shared_ptr<ReactorGLES> reactor, HandleType type);

  static UniqueHandleGLES MakeUntracked(std::shared_ptr<ReactorGLES> reactor,
                                        HandleType type);

  UniqueHandleGLES(std::shared_ptr<ReactorGLES> reactor, HandleGLES handle);

  ~UniqueHandleGLES();

  UniqueHandleGLES(UniqueHandleGLES&&);

  UniqueHandleGLES(const UniqueHandleGLES&) = delete;

  UniqueHandleGLES& operator=(const UniqueHandleGLES&) = delete;

  const HandleGLES& Get() const;

  bool IsValid() const;

 private:
  std::shared_ptr<ReactorGLES> reactor_ = nullptr;
  HandleGLES handle_ = HandleGLES::DeadHandle();
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_GLES_UNIQUE_HANDLE_GLES_H_
