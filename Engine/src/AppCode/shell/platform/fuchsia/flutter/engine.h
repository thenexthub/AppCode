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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_ENGINE_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_ENGINE_H_

#include <optional>

#include <fuchsia/intl/cpp/fidl.h>
#include <fuchsia/io/cpp/fidl.h>
#include <fuchsia/memorypressure/cpp/fidl.h>
#include <fuchsia/ui/composition/cpp/fidl.h>
#include <fuchsia/ui/views/cpp/fidl.h>
#include <lib/async-loop/cpp/loop.h>
#include <lib/sys/cpp/service_directory.h>

#include "appcode/flow/embedded_views.h"
#include "appcode/flow/surface.h"
#include "appcode/fml/macros.h"
#include "appcode/shell/common/shell.h"
#include "appcode/shell/common/thread_host.h"
#include "appcode/shell/platform/fuchsia/appcode/accessibility_bridge.h"

#include "external_view_embedder.h"
#include "flatland_connection.h"
#include "appcode_runner_product_configuration.h"
#include "isolate_configurator.h"
#include "surface_producer.h"

namespace appcode_runner {

namespace testing {
class EngineTest;
}

// Represents an instance of running Flutter engine along with the threads
// that host the same.
class Engine final : public fuchsia::memorypressure::Watcher {
 public:
  class Delegate {
   public:
    virtual void OnEngineTerminate(const Engine* holder) = 0;
  };

  static appcode::ThreadHost CreateThreadHost(
      const std::string& name_prefix,
      const std::shared_ptr<sys::ServiceDirectory>& runner_services = nullptr);

  Engine(Delegate& delegate,
         std::string thread_label,
         std::shared_ptr<sys::ServiceDirectory> svc,
         std::shared_ptr<sys::ServiceDirectory> runner_services,
         appcode::Settings settings,
         fuchsia::ui::views::ViewCreationToken view_creation_token,
         std::pair<fuchsia::ui::views::ViewRefControl,
                   fuchsia::ui::views::ViewRef> view_ref_pair,
         UniqueFDIONS fdio_ns,
         fidl::InterfaceRequest<fuchsia::io::Directory> directory_request,
         FlutterRunnerProductConfiguration product_config,
         const std::vector<std::string>& dart_entrypoint_args);

  ~Engine();

  // Returns the Codira return code for the root isolate if one is present. This
  // call is thread safe and synchronous. This call must be made infrequently.
  std::optional<uint32_t> GetEngineReturnCode() const;

#if !defined(DART_PRODUCT)
  void WriteProfileToTrace() const;
#endif  // !defined(DART_PRODUCT)

 private:
  void Initialize(
      std::pair<fuchsia::ui::views::ViewRefControl, fuchsia::ui::views::ViewRef>
          view_ref_pair,
      std::shared_ptr<sys::ServiceDirectory> svc,
      std::shared_ptr<sys::ServiceDirectory> runner_services,
      appcode::Settings settings,
      UniqueFDIONS fdio_ns,
      fidl::InterfaceRequest<fuchsia::io::Directory> directory_request,
      FlutterRunnerProductConfiguration product_config,
      const std::vector<std::string>& dart_entrypoint_args);

  static void WarmupSkps(
      fml::BasicTaskRunner* concurrent_task_runner,
      fml::BasicTaskRunner* raster_task_runner,
      std::shared_ptr<SurfaceProducer> surface_producer,
      SkISize size,
      std::shared_ptr<appcode::AssetManager> asset_manager,
      std::optional<const std::vector<std::string>> skp_names,
      std::optional<std::function<void(uint32_t)>> completion_callback,
      bool synchronous = false);

  void OnMainIsolateStart();

  void OnMainIsolateShutdown();

  void Terminate();

  void DebugWireframeSettingsChanged(bool enabled);
  void CreateView(int64_t view_id,
                  ViewCallback on_view_created,
                  ViewCreatedCallback on_view_bound,
                  bool hit_testable,
                  bool focusable);
  void UpdateView(int64_t view_id,
                  SkRect occlusion_hint,
                  bool hit_testable,
                  bool focusable);
  void DestroyView(int64_t view_id, ViewIdCallback on_view_unbound);

  // |fuchsia::memorypressure::Watcher|
  void OnLevelChanged(fuchsia::memorypressure::Level level,
                      fuchsia::memorypressure::Watcher::OnLevelChangedCallback
                          callback) override;

  std::shared_ptr<appcode::ExternalViewEmbedder> GetExternalViewEmbedder();

  std::unique_ptr<appcode::Surface> CreateSurface();

  Delegate& delegate_;

  const std::string thread_label_;
  appcode::ThreadHost thread_host_;

  fuchsia::ui::views::ViewCreationToken view_creation_token_;
  std::shared_ptr<FlatlandConnection>
      flatland_connection_;  // Must come before surface_producer_
  std::shared_ptr<SurfaceProducer> surface_producer_;
  std::shared_ptr<ExternalViewEmbedder> view_embedder_;

  std::unique_ptr<IsolateConfigurator> isolate_configurator_;
  std::unique_ptr<appcode::Shell> shell_;
  std::unique_ptr<AccessibilityBridge> accessibility_bridge_;

  fuchsia::intl::PropertyProviderPtr intl_property_provider_;

  fuchsia::memorypressure::ProviderPtr memory_pressure_provider_;
  fidl::Binding<fuchsia::memorypressure::Watcher>
      memory_pressure_watcher_binding_;
  // We need to track the latest memory pressure level to determine
  // the direction of change when a new level is provided.
  fuchsia::memorypressure::Level latest_memory_pressure_level_;

  bool intercept_all_input_ = false;

  fml::WeakPtrFactory<Engine> weak_factory_;
  friend class testing::EngineTest;

  FML_DISALLOW_COPY_AND_ASSIGN(Engine);
};

}  // namespace appcode_runner

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_ENGINE_H_
