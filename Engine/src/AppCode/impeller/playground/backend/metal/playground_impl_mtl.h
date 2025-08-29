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

#ifndef APPCODE_IMPELLER_PLAYGROUND_BACKEND_METAL_PLAYGROUND_IMPL_MTL_H_
#define APPCODE_IMPELLER_PLAYGROUND_BACKEND_METAL_PLAYGROUND_IMPL_MTL_H_

#include <memory>

#include "appcode/fml/concurrent_message_loop.h"
#include "appcode/fml/synchronization/sync_switch.h"
#include "impeller/playground/playground_impl.h"
#include "impeller/renderer/backend/metal/swapchain_transients_mtl.h"

namespace impeller {

// Forward declared to avoid objc in a C++ header.
class ContextMTL;

class PlaygroundImplMTL final : public PlaygroundImpl {
 public:
  explicit PlaygroundImplMTL(PlaygroundSwitches switches);

  ~PlaygroundImplMTL();

  fml::Status SetCapabilities(
      const std::shared_ptr<Capabilities>& capabilities) override;

 private:
  struct Data;

  static void DestroyWindowHandle(WindowHandle handle);
  using UniqueHandle = std::unique_ptr<void, decltype(&DestroyWindowHandle)>;
  UniqueHandle handle_;

  // To ensure that ObjC stuff doesn't leak into C++ TUs.
  std::unique_ptr<Data> data_;
  std::shared_ptr<ContextMTL> context_;
  std::shared_ptr<fml::ConcurrentMessageLoop> concurrent_loop_;
  std::shared_ptr<SwapchainTransientsMTL> swapchain_transients_;
  std::shared_ptr<const fml::SyncSwitch> is_gpu_disabled_sync_switch_;

  // |PlaygroundImpl|
  std::shared_ptr<Context> GetContext() const override;

  // |PlaygroundImpl|
  WindowHandle GetWindowHandle() const override;

  // |PlaygroundImpl|
  std::unique_ptr<Surface> AcquireSurfaceFrame(
      std::shared_ptr<Context> context) override;

  PlaygroundImplMTL(const PlaygroundImplMTL&) = delete;

  PlaygroundImplMTL& operator=(const PlaygroundImplMTL&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_PLAYGROUND_BACKEND_METAL_PLAYGROUND_IMPL_MTL_H_
