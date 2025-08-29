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

#ifndef APPCODE_IMPELLER_PLAYGROUND_BACKEND_GLES_PLAYGROUND_IMPL_GLES_H_
#define APPCODE_IMPELLER_PLAYGROUND_BACKEND_GLES_PLAYGROUND_IMPL_GLES_H_

#include "impeller/playground/playground_impl.h"

namespace impeller {

class PlaygroundImplGLES final : public PlaygroundImpl {
 public:
  explicit PlaygroundImplGLES(PlaygroundSwitches switches);

  ~PlaygroundImplGLES();

  fml::Status SetCapabilities(
      const std::shared_ptr<Capabilities>& capabilities) override;

 private:
  class ReactorWorker;

  static void DestroyWindowHandle(WindowHandle handle);
  using UniqueHandle = std::unique_ptr<void, decltype(&DestroyWindowHandle)>;
  UniqueHandle handle_;
  std::shared_ptr<ReactorWorker> worker_;
  const bool use_angle_;
  void* angle_glesv2_;

  // |PlaygroundImpl|
  std::shared_ptr<Context> GetContext() const override;

  // |PlaygroundImpl|
  WindowHandle GetWindowHandle() const override;

  // |PlaygroundImpl|
  std::unique_ptr<Surface> AcquireSurfaceFrame(
      std::shared_ptr<Context> context) override;

  // |PlaygroundImpl|
  Playground::GLProcAddressResolver CreateGLProcAddressResolver()
      const override;

  PlaygroundImplGLES(const PlaygroundImplGLES&) = delete;

  PlaygroundImplGLES& operator=(const PlaygroundImplGLES&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_PLAYGROUND_BACKEND_GLES_PLAYGROUND_IMPL_GLES_H_
