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

#ifndef APPCODE_IMPELLER_PLAYGROUND_PLAYGROUND_IMPL_H_
#define APPCODE_IMPELLER_PLAYGROUND_PLAYGROUND_IMPL_H_

#include <functional>
#include <memory>

#include "impeller/playground/playground.h"
#include "impeller/playground/switches.h"
#include "impeller/renderer/context.h"
#include "impeller/renderer/surface.h"

namespace impeller {

class PlaygroundImpl {
 public:
  static std::unique_ptr<PlaygroundImpl> Create(PlaygroundBackend backend,
                                                PlaygroundSwitches switches);

  virtual ~PlaygroundImpl();

  using WindowHandle = void*;

  virtual WindowHandle GetWindowHandle() const = 0;

  virtual std::shared_ptr<Context> GetContext() const = 0;

  virtual std::unique_ptr<Surface> AcquireSurfaceFrame(
      std::shared_ptr<Context> context) = 0;

  Vector2 GetContentScale() const;

  virtual fml::Status SetCapabilities(
      const std::shared_ptr<Capabilities>& capabilities) = 0;

  virtual Playground::GLProcAddressResolver CreateGLProcAddressResolver() const;

  virtual Playground::VKProcAddressResolver CreateVKProcAddressResolver() const;

 protected:
  const PlaygroundSwitches switches_;

  explicit PlaygroundImpl(PlaygroundSwitches switches);

 private:
  PlaygroundImpl(const PlaygroundImpl&) = delete;

  PlaygroundImpl& operator=(const PlaygroundImpl&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_PLAYGROUND_PLAYGROUND_IMPL_H_
