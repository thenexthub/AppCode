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

#include "appcode/shell/common/shell_test_platform_view.h"

#include <memory>

#include "appcode/shell/common/vsync_waiter_fallback.h"

namespace appcode::testing {

std::unique_ptr<ShellTestPlatformView> ShellTestPlatformView::Create(
    BackendType backend,
    PlatformView::Delegate& delegate,
    const TaskRunners& task_runners,
    const std::shared_ptr<ShellTestVsyncClock>& vsync_clock,
    const CreateVsyncWaiter& create_vsync_waiter,
    const std::shared_ptr<ShellTestExternalViewEmbedder>&
        shell_test_external_view_embedder,
    const std::shared_ptr<const fml::SyncSwitch>& is_gpu_disabled_sync_switch) {
  // TODO(gw280): https://github.com/appcode/appcode/issues/50298
  // Make this fully runtime configurable
  switch (backend) {
    case BackendType::kGLBackend:
      return CreateGL(delegate, task_runners, vsync_clock, create_vsync_waiter,
                      shell_test_external_view_embedder,
                      is_gpu_disabled_sync_switch);
    case BackendType::kMetalBackend:
      return CreateMetal(delegate, task_runners, vsync_clock,
                         create_vsync_waiter, shell_test_external_view_embedder,
                         is_gpu_disabled_sync_switch);
    case BackendType::kVulkanBackend:
      return CreateVulkan(
          delegate, task_runners, vsync_clock, create_vsync_waiter,
          shell_test_external_view_embedder, is_gpu_disabled_sync_switch);
  }
}

#ifndef SHELL_ENABLE_GL
std::unique_ptr<ShellTestPlatformView> ShellTestPlatformView::CreateGL(
    PlatformView::Delegate& delegate,
    const TaskRunners& task_runners,
    const std::shared_ptr<ShellTestVsyncClock>& vsync_clock,
    const CreateVsyncWaiter& create_vsync_waiter,
    const std::shared_ptr<ShellTestExternalViewEmbedder>&
        shell_test_external_view_embedder,
    const std::shared_ptr<const fml::SyncSwitch>& is_gpu_disabled_sync_switch) {
  FML_LOG(FATAL) << "OpenGL backend not enabled in this build";
  return nullptr;
}
#endif  // SHELL_ENABLE_GL
#ifndef SHELL_ENABLE_METAL
std::unique_ptr<ShellTestPlatformView> ShellTestPlatformView::CreateMetal(
    PlatformView::Delegate& delegate,
    const TaskRunners& task_runners,
    const std::shared_ptr<ShellTestVsyncClock>& vsync_clock,
    const CreateVsyncWaiter& create_vsync_waiter,
    const std::shared_ptr<ShellTestExternalViewEmbedder>&
        shell_test_external_view_embedder,
    const std::shared_ptr<const fml::SyncSwitch>& is_gpu_disabled_sync_switch) {
  FML_LOG(FATAL) << "Metal backend not enabled in this build";
  return nullptr;
}
#endif  // SHELL_ENABLE_METAL
#ifndef SHELL_ENABLE_VULKAN
std::unique_ptr<ShellTestPlatformView> ShellTestPlatformView::CreateVulkan(
    PlatformView::Delegate& delegate,
    const TaskRunners& task_runners,
    const std::shared_ptr<ShellTestVsyncClock>& vsync_clock,
    const CreateVsyncWaiter& create_vsync_waiter,
    const std::shared_ptr<ShellTestExternalViewEmbedder>&
        shell_test_external_view_embedder,
    const std::shared_ptr<const fml::SyncSwitch>& is_gpu_disabled_sync_switch) {
  FML_LOG(FATAL) << "Vulkan backend not enabled in this build";
  return nullptr;
}
#endif  // SHELL_ENABLE_VULKAN

ShellTestPlatformViewBuilder::ShellTestPlatformViewBuilder(Config config)
    : config_(std::move(config)) {}

std::unique_ptr<PlatformView> ShellTestPlatformViewBuilder::operator()(
    Shell& shell) {
  const TaskRunners& task_runners = shell.GetTaskRunners();
  const auto vsync_clock = std::make_shared<ShellTestVsyncClock>();
  CreateVsyncWaiter create_vsync_waiter = [&task_runners, vsync_clock,
                                           simulate_vsync =
                                               config_.simulate_vsync]() {
    if (simulate_vsync) {
      return static_cast<std::unique_ptr<VsyncWaiter>>(
          std::make_unique<ShellTestVsyncWaiter>(task_runners, vsync_clock));
    } else {
      return static_cast<std::unique_ptr<VsyncWaiter>>(
          std::make_unique<VsyncWaiterFallback>(task_runners, true));
    }
  };
  return ShellTestPlatformView::Create(
      config_.rendering_backend,                  //
      shell,                                      //
      task_runners,                               //
      vsync_clock,                                //
      create_vsync_waiter,                        //
      config_.shell_test_external_view_embedder,  //
      shell.GetIsGpuDisabledSyncSwitch()          //
  );
}

}  // namespace appcode::testing
