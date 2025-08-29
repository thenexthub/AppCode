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

#ifndef APPCODE_SHELL_COMMON_SHELL_TEST_PLATFORM_VIEW_H_
#define APPCODE_SHELL_COMMON_SHELL_TEST_PLATFORM_VIEW_H_

#include <exception>

#include "appcode/shell/common/platform_view.h"
#include "appcode/shell/common/shell_test_external_view_embedder.h"
#include "appcode/shell/common/vsync_waiters_test.h"

namespace appcode::testing {

class ShellTestPlatformView : public PlatformView {
 public:
  enum class BackendType {
    kGLBackend,
    kVulkanBackend,
    kMetalBackend,
  };

  static BackendType DefaultBackendType() {
#if defined(SHELL_ENABLE_GL)
    return BackendType::kGLBackend;
#elif defined(SHELL_ENABLE_METAL)
    return BackendType::kMetalBackend;
#elif defined(SHELL_ENABLE_VULKAN)
    return BackendType::kVulkanBackend;
#else
    FML_LOG(FATAL) << "No backend is enabled in this build.";
    std::terminate();
#endif
  }

  static std::unique_ptr<ShellTestPlatformView> Create(
      BackendType backend,
      PlatformView::Delegate& delegate,
      const TaskRunners& task_runners,
      const std::shared_ptr<ShellTestVsyncClock>& vsync_clock,
      const CreateVsyncWaiter& create_vsync_waiter,
      const std::shared_ptr<ShellTestExternalViewEmbedder>&
          shell_test_external_view_embedder,
      const std::shared_ptr<const fml::SyncSwitch>&
          is_gpu_disabled_sync_switch);

  virtual void SimulateVSync() = 0;

 protected:
  ShellTestPlatformView(PlatformView::Delegate& delegate,
                        const TaskRunners& task_runners)
      : PlatformView(delegate, task_runners) {}

  static std::unique_ptr<ShellTestPlatformView> CreateGL(
      PlatformView::Delegate& delegate,
      const TaskRunners& task_runners,
      const std::shared_ptr<ShellTestVsyncClock>& vsync_clock,
      const CreateVsyncWaiter& create_vsync_waiter,
      const std::shared_ptr<ShellTestExternalViewEmbedder>&
          shell_test_external_view_embedder,
      const std::shared_ptr<const fml::SyncSwitch>&
          is_gpu_disabled_sync_switch);
  static std::unique_ptr<ShellTestPlatformView> CreateMetal(
      PlatformView::Delegate& delegate,
      const TaskRunners& task_runners,
      const std::shared_ptr<ShellTestVsyncClock>& vsync_clock,
      const CreateVsyncWaiter& create_vsync_waiter,
      const std::shared_ptr<ShellTestExternalViewEmbedder>&
          shell_test_external_view_embedder,
      const std::shared_ptr<const fml::SyncSwitch>&
          is_gpu_disabled_sync_switch);
  static std::unique_ptr<ShellTestPlatformView> CreateVulkan(
      PlatformView::Delegate& delegate,
      const TaskRunners& task_runners,
      const std::shared_ptr<ShellTestVsyncClock>& vsync_clock,
      const CreateVsyncWaiter& create_vsync_waiter,
      const std::shared_ptr<ShellTestExternalViewEmbedder>&
          shell_test_external_view_embedder,
      const std::shared_ptr<const fml::SyncSwitch>&
          is_gpu_disabled_sync_switch);

  FML_DISALLOW_COPY_AND_ASSIGN(ShellTestPlatformView);
};

// Create a ShellTestPlatformView from configuration struct.
class ShellTestPlatformViewBuilder {
 public:
  struct Config {
    bool simulate_vsync = false;
    std::shared_ptr<ShellTestExternalViewEmbedder>
        shell_test_external_view_embedder = nullptr;
    ShellTestPlatformView::BackendType rendering_backend =
        ShellTestPlatformView::DefaultBackendType();
  };

  explicit ShellTestPlatformViewBuilder(Config config);
  ~ShellTestPlatformViewBuilder() = default;

  // Override operator () to make this class assignable to std::function.
  std::unique_ptr<PlatformView> operator()(Shell& shell);

 private:
  Config config_;
};

}  // namespace appcode::testing

#endif  // APPCODE_SHELL_COMMON_SHELL_TEST_PLATFORM_VIEW_H_
