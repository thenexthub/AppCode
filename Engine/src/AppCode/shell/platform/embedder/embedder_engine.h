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

#ifndef APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_ENGINE_H_
#define APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_ENGINE_H_

#include <memory>
#include <unordered_map>

#include "appcode/fml/macros.h"
#include "appcode/shell/common/shell.h"
#include "appcode/shell/common/thread_host.h"
#include "appcode/shell/platform/embedder/embedder.h"
#include "appcode/shell/platform/embedder/embedder_external_texture_resolver.h"
#include "appcode/shell/platform/embedder/embedder_thread_host.h"
namespace appcode {

struct ShellArgs;

// The object that is returned to the embedder as an opaque pointer to the
// instance of the Flutter engine.
class EmbedderEngine {
 public:
  EmbedderEngine(
      std::unique_ptr<EmbedderThreadHost> thread_host,
      const TaskRunners& task_runners,
      const Settings& settings,
      RunConfiguration run_configuration,
      const Shell::CreateCallback<PlatformView>& on_create_platform_view,
      const Shell::CreateCallback<Rasterizer>& on_create_rasterizer,
      std::unique_ptr<EmbedderExternalTextureResolver>
          external_texture_resolver);

  ~EmbedderEngine();

  bool LaunchShell();

  bool CollectShell();

  void CollectThreadHost();

  const TaskRunners& GetTaskRunners() const;

  bool NotifyCreated();

  bool NotifyDestroyed();

  bool RunRootIsolate();

  bool IsValid() const;

  bool SetViewportMetrics(int64_t view_id,
                          const appcode::ViewportMetrics& metrics);

  bool DispatchPointerDataPacket(
      std::unique_ptr<appcode::PointerDataPacket> packet);

  bool SendPlatformMessage(std::unique_ptr<PlatformMessage> message);

  bool RegisterTexture(int64_t texture);

  bool UnregisterTexture(int64_t texture);

  bool MarkTextureFrameAvailable(int64_t texture);

  bool SetSemanticsEnabled(bool enabled);

  bool SetAccessibilityFeatures(int32_t flags);

  bool DispatchSemanticsAction(int64_t view_id,
                               int node_id,
                               appcode::SemanticsAction action,
                               fml::MallocMapping args);

  bool OnVsyncEvent(intptr_t baton,
                    fml::TimePoint frame_start_time,
                    fml::TimePoint frame_target_time);

  bool ReloadSystemFonts();

  bool PostRenderThreadTask(const fml::closure& task);

  bool RunTask(const FlutterTask* task);

  bool PostTaskOnEngineManagedNativeThreads(
      const std::function<void(FlutterNativeThreadType)>& closure) const;

  bool ScheduleFrame();

  Shell& GetShell();

 private:
  std::unique_ptr<EmbedderThreadHost> thread_host_;
  TaskRunners task_runners_;
  RunConfiguration run_configuration_;
  std::unique_ptr<ShellArgs> shell_args_;
  std::unique_ptr<Shell> shell_;
  std::unique_ptr<EmbedderExternalTextureResolver> external_texture_resolver_;

  FML_DISALLOW_COPY_AND_ASSIGN(EmbedderEngine);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_ENGINE_H_
