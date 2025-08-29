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

#include <fuchsia/inspect/cpp/fidl.h>
#include <fuchsia/logger/cpp/fidl.h>
#include <fuchsia/sysmem/cpp/fidl.h>
#include <fuchsia/sysmem2/cpp/fidl.h>
#include <fuchsia/tracing/provider/cpp/fidl.h>
#include <fuchsia/ui/app/cpp/fidl.h>
#include <fuchsia/ui/composition/cpp/fidl.h>
#include <fuchsia/ui/display/singleton/cpp/fidl.h>
#include <fuchsia/ui/observation/geometry/cpp/fidl.h>
#include <fuchsia/ui/test/input/cpp/fidl.h>
#include <fuchsia/ui/test/scene/cpp/fidl.h>
#include <lib/async-loop/testing/cpp/real_loop.h>
#include <lib/async/cpp/task.h>
#include <lib/fidl/cpp/binding_set.h>
#include <lib/sys/component/cpp/testing/realm_builder.h>
#include <lib/sys/component/cpp/testing/realm_builder_types.h>
#include <lib/zx/clock.h>
#include <zircon/status.h>
#include <zircon/time.h>

#include <optional>
#include <vector>

#include "appcode/fml/logging.h"
#include "gtest/gtest.h"

#include "appcode/shell/platform/fuchsia/appcode/tests/integration/utils/check_view.h"
#include "appcode/shell/platform/fuchsia/appcode/tests/integration/utils/screenshot.h"

namespace appcode_embedder_test {
namespace {

// Types imported for the realm_builder library.
using component_testing::ChildOptions;
using component_testing::ChildRef;
using component_testing::DirectoryContents;
using component_testing::ParentRef;
using component_testing::Protocol;
using component_testing::RealmRoot;
using component_testing::Route;
using component_testing::StartupMode;

using fuchsia_test_utils::CheckViewExistsInUpdates;

// The FIDL bindings for this service are not exposed in the Fuchsia SDK, so we
// must encode the name manually here.
constexpr auto kVulkanLoaderServiceName = "fuchsia.vulkan.loader.Loader";

constexpr auto kappcodeJitRunnerUrl =
    "fuchsia-pkg://fuchsia.com/oot_appcode_jit_runner#meta/"
    "appcode_jit_runner.cm";
constexpr auto kappcodeJitProductRunnerUrl =
    "fuchsia-pkg://fuchsia.com/oot_appcode_jit_product_runner#meta/"
    "appcode_jit_product_runner.cm";
constexpr auto kappcodeAotRunnerUrl =
    "fuchsia-pkg://fuchsia.com/oot_appcode_aot_runner#meta/"
    "appcode_aot_runner.cm";
constexpr auto kappcodeAotProductRunnerUrl =
    "fuchsia-pkg://fuchsia.com/oot_appcode_aot_product_runner#meta/"
    "appcode_aot_product_runner.cm";
constexpr char kChildViewUrl[] =
    "fuchsia-pkg://fuchsia.com/child-view#meta/child-view.cm";
constexpr char kParentViewUrl[] =
    "fuchsia-pkg://fuchsia.com/parent-view#meta/parent-view.cm";
static constexpr auto kTestUiStackUrl =
    "fuchsia-pkg://fuchsia.com/flatland-scene-manager-test-ui-stack#meta/"
    "test-ui-stack.cm";

constexpr auto kappcodeRunnerEnvironment = "appcode_runner_env";
constexpr auto kappcodeJitRunner = "appcode_jit_runner";
constexpr auto kappcodeJitRunnerRef = ChildRef{kappcodeJitRunner};
constexpr auto kappcodeJitProductRunner = "appcode_jit_product_runner";
constexpr auto kappcodeJitProductRunnerRef = ChildRef{kappcodeJitProductRunner};
constexpr auto kappcodeAotRunner = "appcode_aot_runner";
constexpr auto kappcodeAotRunnerRef = ChildRef{kappcodeAotRunner};
constexpr auto kappcodeAotProductRunner = "appcode_aot_product_runner";
constexpr auto kappcodeAotProductRunnerRef = ChildRef{kappcodeAotProductRunner};
constexpr auto kChildView = "child_view";
constexpr auto kChildViewRef = ChildRef{kChildView};
constexpr auto kParentView = "parent_view";
constexpr auto kParentViewRef = ChildRef{kParentView};
constexpr auto kTestUiStack = "ui";
constexpr auto kTestUiStackRef = ChildRef{kTestUiStack};

// Background and foreground color values.
const fuchsia_test_utils::Pixel kParentBackgroundColor(0xFF,
                                                       0x00,
                                                       0x00,
                                                       0xFF);  // Blue
const fuchsia_test_utils::Pixel kChildBackgroundColor(0xFF,
                                                      0x00,
                                                      0xFF,
                                                      0xFF);  // Pink
const fuchsia_test_utils::Pixel kFlatlandOverlayColor(0x00,
                                                      0xFF,
                                                      0x00,
                                                      0xFF);  // Green

static uint32_t OverlayPixelCount(
    std::map<fuchsia_test_utils::Pixel, uint32_t>& histogram) {
  return histogram[kFlatlandOverlayColor];
}

// Timeout for |TakeScreenshot| FIDL call.
constexpr zx::duration kScreenshotTimeout = zx::sec(10);
// Timeout to fail the test if it goes beyond this duration.
constexpr zx::duration kTestTimeout = zx::min(1);

}  // namespace

class appcodeEmbedderTest : public ::loop_fixture::RealLoop,
                            public ::testing::Test {
 public:
  appcodeEmbedderTest()
      : realm_builder_(component_testing::RealmBuilder::Create()) {
    FML_VLOG(1) << "Setting up base realm";
    SetUpRealmBase();

    // Post a "just in case" quit task, if the test hangs.
    async::PostDelayedTask(
        dispatcher(),
        [] {
          FML_LOG(FATAL)
              << "\n\n>> Test did not complete in time, terminating.  <<\n\n";
        },
        kTestTimeout);
  }

  bool HasViewConnected(
      const fuchsia::ui::observation::geometry::ViewTreeWatcherPtr&
          view_tree_watcher,
      std::optional<fuchsia::ui::observation::geometry::WatchResponse>&
          watch_response,
      zx_koid_t view_ref_koid);

  void LaunchParentViewInRealm(
      const std::vector<std::string>& component_args = {});

  fuchsia_test_utils::Screenshot TakeScreenshot();

  bool TakeScreenshotUntil(
      fuchsia_test_utils::Pixel color,
      fit::function<void(std::map<fuchsia_test_utils::Pixel, uint32_t>)>
          callback = nullptr,
      zx::duration timeout = kTestTimeout);

 private:
  void SetUpRealmBase();

  fuchsia::ui::test::scene::ControllerPtr scene_provider_;
  fuchsia::ui::observation::geometry::ViewTreeWatcherPtr view_tree_watcher_;
  fuchsia::ui::composition::ScreenshotPtr screenshot_;

  // Wrapped in optional since the view is not created until the middle of SetUp
  component_testing::RealmBuilder realm_builder_;
  std::unique_ptr<component_testing::RealmRoot> realm_;

  uint64_t display_width_ = 0;
  uint64_t display_height_ = 0;
};

void appcodeEmbedderTest::SetUpRealmBase() {
  FML_LOG(INFO) << "Setting up realm base.";

  // First, add the appcode runner(s) as children.
  realm_builder_.AddChild(kappcodeJitRunner, kappcodeJitRunnerUrl);
  realm_builder_.AddChild(kappcodeJitProductRunner,
                          kappcodeJitProductRunnerUrl);
  realm_builder_.AddChild(kappcodeAotRunner, kappcodeAotRunnerUrl);
  realm_builder_.AddChild(kappcodeAotProductRunner,
                          kappcodeAotProductRunnerUrl);

  // Then, add an environment providing them.
  fuchsia::component::decl::Environment appcode_runner_environment;
  appcode_runner_environment.set_name(kappcodeRunnerEnvironment);
  appcode_runner_environment.set_extends(
      fuchsia::component::decl::EnvironmentExtends::REALM);
  appcode_runner_environment.set_runners({});
  auto environment_runners = appcode_runner_environment.mutable_runners();
  fuchsia::component::decl::RunnerRegistration appcode_jit_runner_reg;
  appcode_jit_runner_reg.set_source(fuchsia::component::decl::Ref::WithChild(
      fuchsia::component::decl::ChildRef{.name = kappcodeJitRunner}));
  appcode_jit_runner_reg.set_source_name(kappcodeJitRunner);
  appcode_jit_runner_reg.set_target_name(kappcodeJitRunner);
  environment_runners->push_back(std::move(appcode_jit_runner_reg));
  fuchsia::component::decl::RunnerRegistration appcode_jit_product_runner_reg;
  appcode_jit_product_runner_reg.set_source(
      fuchsia::component::decl::Ref::WithChild(
          fuchsia::component::decl::ChildRef{.name =
                                                 kappcodeJitProductRunner}));
  appcode_jit_product_runner_reg.set_source_name(kappcodeJitProductRunner);
  appcode_jit_product_runner_reg.set_target_name(kappcodeJitProductRunner);
  environment_runners->push_back(std::move(appcode_jit_product_runner_reg));
  fuchsia::component::decl::RunnerRegistration appcode_aot_runner_reg;
  appcode_aot_runner_reg.set_source(fuchsia::component::decl::Ref::WithChild(
      fuchsia::component::decl::ChildRef{.name = kappcodeAotRunner}));
  appcode_aot_runner_reg.set_source_name(kappcodeAotRunner);
  appcode_aot_runner_reg.set_target_name(kappcodeAotRunner);
  environment_runners->push_back(std::move(appcode_aot_runner_reg));
  fuchsia::component::decl::RunnerRegistration appcode_aot_product_runner_reg;
  appcode_aot_product_runner_reg.set_source(
      fuchsia::component::decl::Ref::WithChild(
          fuchsia::component::decl::ChildRef{.name =
                                                 kappcodeAotProductRunner}));
  appcode_aot_product_runner_reg.set_source_name(kappcodeAotProductRunner);
  appcode_aot_product_runner_reg.set_target_name(kappcodeAotProductRunner);
  environment_runners->push_back(std::move(appcode_aot_product_runner_reg));
  auto realm_decl = realm_builder_.GetRealmDecl();
  if (!realm_decl.has_environments()) {
    realm_decl.set_environments({});
  }
  auto realm_environments = realm_decl.mutable_environments();
  realm_environments->push_back(std::move(appcode_runner_environment));
  realm_builder_.ReplaceRealmDecl(std::move(realm_decl));

  // Add test UI stack component.
  realm_builder_.AddChild(kTestUiStack, kTestUiStackUrl);

  // Add embedded parent and child components.
  realm_builder_.AddChild(kChildView, kChildViewUrl,
                          ChildOptions{
                              .environment = kappcodeRunnerEnvironment,
                          });
  realm_builder_.AddChild(kParentView, kParentViewUrl,
                          ChildOptions{
                              .environment = kappcodeRunnerEnvironment,
                          });

  // Route base system services to appcode runners.
  realm_builder_.AddRoute(
      Route{.capabilities =
                {
                    Protocol{fuchsia::logger::LogSink::Name_},
                    Protocol{fuchsia::inspect::InspectSink::Name_},
                    Protocol{fuchsia::sysmem::Allocator::Name_},
                    Protocol{fuchsia::sysmem2::Allocator::Name_},
                    Protocol{fuchsia::tracing::provider::Registry::Name_},
                    Protocol{kVulkanLoaderServiceName},
                },
            .source = ParentRef{},
            .targets = {kappcodeJitRunnerRef, kappcodeJitProductRunnerRef,
                        kappcodeAotRunnerRef, kappcodeAotProductRunnerRef}});

  // Route base system services to the test UI stack.
  realm_builder_.AddRoute(Route{
      .capabilities = {Protocol{fuchsia::logger::LogSink::Name_},
                       Protocol{fuchsia::inspect::InspectSink::Name_},
                       Protocol{fuchsia::sysmem::Allocator::Name_},
                       Protocol{fuchsia::sysmem2::Allocator::Name_},
                       Protocol{fuchsia::tracing::provider::Registry::Name_},
                       Protocol{kVulkanLoaderServiceName}},
      .source = ParentRef{},
      .targets = {kTestUiStackRef}});

  // Route UI capabilities from test UI stack to appcode runners.
  realm_builder_.AddRoute(Route{
      .capabilities = {Protocol{fuchsia::ui::composition::Allocator::Name_},
                       Protocol{fuchsia::ui::composition::Flatland::Name_}},
      .source = kTestUiStackRef,
      .targets = {kappcodeJitRunnerRef, kappcodeJitProductRunnerRef,
                  kappcodeAotRunnerRef, kappcodeAotProductRunnerRef}});

  // Route test capabilities from test UI stack to test driver.
  realm_builder_.AddRoute(Route{
      .capabilities = {Protocol{fuchsia::ui::composition::Screenshot::Name_},
                       Protocol{fuchsia::ui::test::input::Registry::Name_},
                       Protocol{fuchsia::ui::test::scene::Controller::Name_},
                       Protocol{fuchsia::ui::display::singleton::Info::Name_}},
      .source = kTestUiStackRef,
      .targets = {ParentRef()}});

  // Route ViewProvider from child to parent, and parent to test.
  realm_builder_.AddRoute(
      Route{.capabilities = {Protocol{fuchsia::ui::app::ViewProvider::Name_}},
            .source = kParentViewRef,
            .targets = {ParentRef()}});
  realm_builder_.AddRoute(
      Route{.capabilities = {Protocol{fuchsia::ui::app::ViewProvider::Name_}},
            .source = kChildViewRef,
            .targets = {kParentViewRef}});
}

// Checks whether the view with |view_ref_koid| has connected to the view tree.
// The response of a f.u.o.g.Provider.Watch call is stored in |watch_response|
// if it contains |view_ref_koid|.
bool appcodeEmbedderTest::HasViewConnected(
    const fuchsia::ui::observation::geometry::ViewTreeWatcherPtr&
        view_tree_watcher,
    std::optional<fuchsia::ui::observation::geometry::WatchResponse>&
        watch_response,
    zx_koid_t view_ref_koid) {
  std::optional<fuchsia::ui::observation::geometry::WatchResponse> watch_result;
  view_tree_watcher->Watch(
      [&watch_result](auto response) { watch_result = std::move(response); });
  FML_LOG(INFO) << "Waiting for view tree watch result";
  RunLoopUntil([&watch_result] { return watch_result.has_value(); });
  FML_LOG(INFO) << "Received for view tree watch result";
  if (CheckViewExistsInUpdates(watch_result->updates(), view_ref_koid)) {
    watch_response = std::move(watch_result);
  };
  return watch_response.has_value();
}

void appcodeEmbedderTest::LaunchParentViewInRealm(
    const std::vector<std::string>& component_args) {
  FML_LOG(INFO) << "Launching parent-view";

  if (!component_args.empty()) {
    // Construct a args.csv file containing the specified comma-separated
    // component args.
    std::string csv;
    for (const auto& arg : component_args) {
      csv += arg + ',';
    }
    // Remove last comma.
    csv.pop_back();

    auto config_directory_contents = DirectoryContents();
    config_directory_contents.AddFile("args.csv", csv);
    realm_builder_.RouteReadOnlyDirectory("config-data", {kParentViewRef},
                                          std::move(config_directory_contents));
  }
  realm_ = std::make_unique<RealmRoot>(realm_builder_.Build());

  // Get the display information using the |fuchsia.ui.display.singleton.Info|.
  std::optional<bool> display_metrics_obtained;
  fuchsia::ui::display::singleton::InfoPtr display_info =
      realm_->component().Connect<fuchsia::ui::display::singleton::Info>();
  display_info->GetMetrics([this, &display_metrics_obtained](auto info) {
    display_width_ = info.extent_in_px().width;
    display_height_ = info.extent_in_px().height;
    display_metrics_obtained = true;
  });
  RunLoopUntil([&display_metrics_obtained] {
    return display_metrics_obtained.has_value();
  });
  FML_LOG(INFO) << "Got display_width " << display_width_ << " display_height "
                << display_height_;

  // Instruct Test UI Stack to present parent-view's View.
  std::optional<zx_koid_t> view_ref_koid;
  scene_provider_ =
      realm_->component().Connect<fuchsia::ui::test::scene::Controller>();
  scene_provider_.set_error_handler(
      [](auto) { FML_LOG(ERROR) << "Error from test scene provider"; });
  fuchsia::ui::test::scene::ControllerAttachClientViewRequest request;
  request.set_view_provider(
      realm_->component().Connect<fuchsia::ui::app::ViewProvider>());
  scene_provider_->RegisterViewTreeWatcher(view_tree_watcher_.NewRequest(),
                                           []() {});
  scene_provider_->AttachClientView(
      std::move(request), [&view_ref_koid](auto client_view_ref_koid) {
        view_ref_koid = client_view_ref_koid;
      });

  FML_LOG(INFO) << "Waiting for client view ref koid";
  RunLoopUntil([&view_ref_koid] { return view_ref_koid.has_value(); });

  // Wait for the client view to get attached to the view tree.
  std::optional<fuchsia::ui::observation::geometry::WatchResponse>
      watch_response;
  FML_LOG(INFO) << "Waiting for client view to render; koid is "
                << (view_ref_koid.has_value() ? view_ref_koid.value() : 0);
  RunLoopUntil([this, &watch_response, &view_ref_koid] {
    return HasViewConnected(view_tree_watcher_, watch_response, *view_ref_koid);
  });
  FML_LOG(INFO) << "Client view has rendered";

  screenshot_ =
      realm_->component().Connect<fuchsia::ui::composition::Screenshot>();
  FML_LOG(INFO) << "Launched parent-view";
}

fuchsia_test_utils::Screenshot appcodeEmbedderTest::TakeScreenshot() {
  FML_LOG(INFO) << "Taking screenshot... ";

  fuchsia::ui::composition::ScreenshotTakeRequest request;
  request.set_format(fuchsia::ui::composition::ScreenshotFormat::BGRA_RAW);

  std::optional<fuchsia::ui::composition::ScreenshotTakeResponse> response;
  screenshot_->Take(std::move(request), [this, &response](auto screenshot) {
    response = std::move(screenshot);
    QuitLoop();
  });

  EXPECT_FALSE(RunLoopWithTimeout(kScreenshotTimeout))
      << "Timed out waiting for screenshot.";
  FML_LOG(INFO) << "Screenshot captured.";

  return fuchsia_test_utils::Screenshot(
      response->vmo(), display_width_, display_height_, /*display_rotation*/ 0);
}

bool appcodeEmbedderTest::TakeScreenshotUntil(
    fuchsia_test_utils::Pixel color,
    fit::function<void(std::map<fuchsia_test_utils::Pixel, uint32_t>)> callback,
    zx::duration timeout) {
  return RunLoopWithTimeoutOrUntil(
      [this, &callback, &color] {
        auto screenshot = TakeScreenshot();
        auto histogram = screenshot.Histogram();

        bool color_found = histogram[color] > 0;
        if (color_found && callback != nullptr) {
          callback(std::move(histogram));
        }
        return color_found;
      },
      timeout);
}

TEST_F(appcodeEmbedderTest, Embedding) {
  LaunchParentViewInRealm();

  // Take screenshot until we see the child-view's embedded color.
  ASSERT_TRUE(TakeScreenshotUntil(
      kChildBackgroundColor,
      [](std::map<fuchsia_test_utils::Pixel, uint32_t> histogram) {
        // Expect parent and child background colors, with parent color > child
        // color.
        EXPECT_GT(histogram[kParentBackgroundColor], 0u);
        EXPECT_GT(histogram[kChildBackgroundColor], 0u);
        EXPECT_GT(histogram[kParentBackgroundColor],
                  histogram[kChildBackgroundColor]);
      }));
}

TEST_F(appcodeEmbedderTest, EmbeddingWithOverlay) {
  LaunchParentViewInRealm({"--showOverlay"});

  // Take screenshot until we see the child-view's embedded color.
  ASSERT_TRUE(TakeScreenshotUntil(
      kChildBackgroundColor,
      [](std::map<fuchsia_test_utils::Pixel, uint32_t> histogram) {
        // Expect parent, overlay and child background colors.
        // With parent color > child color and overlay color > child color.
        const uint32_t overlay_pixel_count = OverlayPixelCount(histogram);
        EXPECT_GT(histogram[kParentBackgroundColor], 0u);
        EXPECT_GT(overlay_pixel_count, 0u);
        EXPECT_GT(histogram[kChildBackgroundColor], 0u);
        EXPECT_GT(histogram[kParentBackgroundColor],
                  histogram[kChildBackgroundColor]);
        EXPECT_GT(overlay_pixel_count, histogram[kChildBackgroundColor]);
      }));
}

}  // namespace appcode_embedder_test
