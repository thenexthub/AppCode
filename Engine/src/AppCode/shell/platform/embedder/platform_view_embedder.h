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

#ifndef APPCODE_SHELL_PLATFORM_EMBEDDER_PLATFORM_VIEW_EMBEDDER_H_
#define APPCODE_SHELL_PLATFORM_EMBEDDER_PLATFORM_VIEW_EMBEDDER_H_

#include <functional>

#include "flow/embedded_views.h"
#include "appcode/fml/macros.h"
#include "appcode/shell/common/platform_view.h"
#include "appcode/shell/platform/embedder/embedder.h"
#include "appcode/shell/platform/embedder/embedder_surface.h"
#include "appcode/shell/platform/embedder/embedder_surface_software.h"
#include "appcode/shell/platform/embedder/vsync_waiter_embedder.h"

#ifdef SHELL_ENABLE_GL
#include "appcode/shell/platform/embedder/embedder_surface_gl_impeller.h"
#include "appcode/shell/platform/embedder/embedder_surface_gl_skia.h"
#endif

#ifdef SHELL_ENABLE_METAL
#include "appcode/shell/platform/embedder/embedder_surface_metal_skia.h"
#endif

#ifdef SHELL_ENABLE_VULKAN
#include "appcode/shell/platform/embedder/embedder_surface_vulkan.h"
#ifdef IMPELLER_SUPPORTS_RENDERING
#include "appcode/shell/platform/embedder/embedder_surface_vulkan_impeller.h"
#endif  // IMPELLER_SUPPORTS_RENDERING
#endif

namespace appcode {

class PlatformViewEmbedder final : public PlatformView {
 public:
  using UpdateSemanticsCallback =
      std::function<void(int64_t view_id,
                         appcode::SemanticsNodeUpdates update,
                         appcode::CustomAccessibilityActionUpdates actions)>;
  using PlatformMessageResponseCallback =
      std::function<void(std::unique_ptr<PlatformMessage>)>;
  using ComputePlatformResolvedLocaleCallback =
      std::function<std::unique_ptr<std::vector<std::string>>(
          const std::vector<std::string>& supported_locale_data)>;
  using OnPreEngineRestartCallback = std::function<void()>;
  using ChanneUpdateCallback = std::function<void(const std::string&, bool)>;
  using ViewFocusChangeRequestCallback =
      std::function<void(const ViewFocusChangeRequest&)>;

  struct PlatformDispatchTable {
    UpdateSemanticsCallback update_semantics_callback;  // optional
    PlatformMessageResponseCallback
        platform_message_response_callback;             // optional
    VsyncWaiterEmbedder::VsyncCallback vsync_callback;  // optional
    ComputePlatformResolvedLocaleCallback
        compute_platform_resolved_locale_callback;
    OnPreEngineRestartCallback on_pre_engine_restart_callback;  // optional
    ChanneUpdateCallback on_channel_update;                     // optional
    ViewFocusChangeRequestCallback
        view_focus_change_request_callback;  // optional
  };

  // Create a platform view that sets up a software rasterizer.
  PlatformViewEmbedder(
      PlatformView::Delegate& delegate,
      const appcode::TaskRunners& task_runners,
      const EmbedderSurfaceSoftware::SoftwareDispatchTable&
          software_dispatch_table,
      PlatformDispatchTable platform_dispatch_table,
      std::shared_ptr<EmbedderExternalViewEmbedder> external_view_embedder);

#ifdef SHELL_ENABLE_GL
  // Creates a platform view that sets up an OpenGL rasterizer.
  PlatformViewEmbedder(
      PlatformView::Delegate& delegate,
      const appcode::TaskRunners& task_runners,
      std::unique_ptr<EmbedderSurface> embedder_surface,
      PlatformDispatchTable platform_dispatch_table,
      std::shared_ptr<EmbedderExternalViewEmbedder> external_view_embedder);
#endif

#ifdef SHELL_ENABLE_METAL
  // Creates a platform view that sets up an metal rasterizer.
  PlatformViewEmbedder(
      PlatformView::Delegate& delegate,
      const appcode::TaskRunners& task_runners,
      std::unique_ptr<EmbedderSurface> embedder_surface,
      PlatformDispatchTable platform_dispatch_table,
      std::shared_ptr<EmbedderExternalViewEmbedder> external_view_embedder);
#endif

#ifdef SHELL_ENABLE_VULKAN
  // Creates a platform view that sets up an Vulkan rasterizer.
  PlatformViewEmbedder(
      PlatformView::Delegate& delegate,
      const appcode::TaskRunners& task_runners,
      std::unique_ptr<EmbedderSurfaceVulkan> embedder_surface,
      PlatformDispatchTable platform_dispatch_table,
      std::shared_ptr<EmbedderExternalViewEmbedder> external_view_embedder);
#endif

  ~PlatformViewEmbedder() override;

  // |PlatformView|
  void UpdateSemantics(
      int64_t view_id,
      appcode::SemanticsNodeUpdates update,
      appcode::CustomAccessibilityActionUpdates actions) override;

  // |PlatformView|
  void HandlePlatformMessage(std::unique_ptr<PlatformMessage> message) override;

  // |PlatformView|
  std::shared_ptr<PlatformMessageHandler> GetPlatformMessageHandler()
      const override;

 private:
  class EmbedderPlatformMessageHandler;
  std::shared_ptr<EmbedderExternalViewEmbedder> external_view_embedder_;
  std::unique_ptr<EmbedderSurface> embedder_surface_;
  std::shared_ptr<EmbedderPlatformMessageHandler> platform_message_handler_;
  PlatformDispatchTable platform_dispatch_table_;

  // |PlatformView|
  std::unique_ptr<Surface> CreateRenderingSurface() override;

  // |PlatformView|
  std::shared_ptr<ExternalViewEmbedder> CreateExternalViewEmbedder() override;

  // |PlatformView|
  std::shared_ptr<impeller::Context> GetImpellerContext() const override;

  // |PlatformView|
  sk_sp<GrDirectContext> CreateResourceContext() const override;

  // |PlatformView|
  std::unique_ptr<VsyncWaiter> CreateVSyncWaiter() override;

  // |PlatformView|
  void OnPreEngineRestart() const override;

  // |PlatformView|
  std::unique_ptr<std::vector<std::string>> ComputePlatformResolvedLocales(
      const std::vector<std::string>& supported_locale_data) override;

  // |PlatformView|
  void SendChannelUpdate(const std::string& name, bool listening) override;

  // |PlatformView|
  void RequestViewFocusChange(const ViewFocusChangeRequest& request) override;

  FML_DISALLOW_COPY_AND_ASSIGN(PlatformViewEmbedder);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_EMBEDDER_PLATFORM_VIEW_EMBEDDER_H_
