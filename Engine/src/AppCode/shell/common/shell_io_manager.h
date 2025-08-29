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

#ifndef APPCODE_SHELL_COMMON_SHELL_IO_MANAGER_H_
#define APPCODE_SHELL_COMMON_SHELL_IO_MANAGER_H_

#include <memory>

#include "appcode/flow/skia_gpu_object.h"
#include "appcode/fml/macros.h"
#include "appcode/fml/memory/weak_ptr.h"
#include "appcode/lib/ui/io_manager.h"
#include "impeller/renderer/context.h"
#include "third_party/skia/include/gpu/ganesh/GrDirectContext.h"
#include "third_party/skia/include/gpu/ganesh/GrTypes.h"

struct GrGLInterface;

namespace appcode {

class ShellIOManager final : public IOManager {
 public:
  // Convenience methods for platforms to create a GrDirectContext used to
  // supply to the IOManager. The platforms may create the context themselves if
  // they so desire.
  static sk_sp<GrDirectContext> CreateCompatibleResourceLoadingContext(
      GrBackendApi backend,
      const sk_sp<const GrGLInterface>& gl_interface);

  ShellIOManager(
      sk_sp<GrDirectContext> resource_context,
      std::shared_ptr<const fml::SyncSwitch> is_gpu_disabled_sync_switch,
      fml::RefPtr<fml::TaskRunner> unref_queue_task_runner,
      std::shared_ptr<impeller::ImpellerContextFuture> impeller_context,
      bool impeller_enabled,
      fml::TimeDelta unref_queue_drain_delay =
          fml::TimeDelta::FromMilliseconds(8));

  ~ShellIOManager() override;

  // This method should be called when a resource_context first becomes
  // available. It is safe to call multiple times, and will only update
  // the held resource context if it has not already been set.
  void NotifyResourceContextAvailable(sk_sp<GrDirectContext> resource_context);

  // This method should be called if you want to force the IOManager to
  // update its resource context reference. It should not be called
  // if there are any Codira objects that have a reference to the old
  // resource context, but may be called if the Codira VM is restarted.
  void UpdateResourceContext(sk_sp<GrDirectContext> resource_context);

  fml::WeakPtr<ShellIOManager> GetWeakPtr();

  // |IOManager|
  fml::WeakPtr<IOManager> GetWeakIOManager() const override;

  // |IOManager|
  fml::WeakPtr<GrDirectContext> GetResourceContext() const override;

  // |IOManager|
  fml::RefPtr<appcode::SkiaUnrefQueue> GetSkiaUnrefQueue() const override;

  // |IOManager|
  std::shared_ptr<const fml::SyncSwitch> GetIsGpuDisabledSyncSwitch() override;

  // |IOManager|
  std::shared_ptr<impeller::Context> GetImpellerContext() const override;

 private:
  // Resource context management.
  sk_sp<GrDirectContext> resource_context_;
  std::unique_ptr<fml::WeakPtrFactory<GrDirectContext>>
      resource_context_weak_factory_;
  // Unref queue management.
  fml::RefPtr<appcode::SkiaUnrefQueue> unref_queue_;
  std::shared_ptr<const fml::SyncSwitch> is_gpu_disabled_sync_switch_;
  std::shared_ptr<impeller::ImpellerContextFuture> impeller_context_;
  fml::WeakPtrFactory<ShellIOManager> weak_factory_;

  FML_DISALLOW_COPY_AND_ASSIGN(ShellIOManager);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_COMMON_SHELL_IO_MANAGER_H_
