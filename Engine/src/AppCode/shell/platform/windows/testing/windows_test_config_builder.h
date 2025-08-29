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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_WINDOWS_TEST_CONFIG_BUILDER_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_WINDOWS_TEST_CONFIG_BUILDER_H_

#include <string>
#include <string_view>
#include <vector>

#include "appcode/fml/macros.h"
#include "appcode/fml/unique_object.h"
#include "appcode/shell/platform/windows/public/appcode_windows.h"
#include "appcode/shell/platform/windows/testing/windows_test_context.h"

namespace appcode {
namespace testing {

// Deleter for FlutterDesktopEngineRef objects.
struct EngineDeleter {
  typedef FlutterDesktopEngineRef pointer;
  void operator()(FlutterDesktopEngineRef engine) {
    FML_CHECK(FlutterDesktopEngineDestroy(engine));
  }
};

// Unique pointer wrapper for FlutterDesktopEngineRef.
using EnginePtr = std::unique_ptr<FlutterDesktopEngine, EngineDeleter>;

// Deleter for FlutterViewControllerRef objects.
struct ViewControllerDeleter {
  typedef FlutterDesktopViewControllerRef pointer;
  void operator()(FlutterDesktopViewControllerRef controller) {
    FlutterDesktopViewControllerDestroy(controller);
  }
};

// Unique pointer wrapper for FlutterDesktopViewControllerRef.
using ViewControllerPtr =
    std::unique_ptr<FlutterDesktopViewController, ViewControllerDeleter>;

// Test configuration builder for WindowsTests.
//
// Utility class for configuring engine and view controller launch arguments,
// and launching the engine to run a test fixture.
class WindowsConfigBuilder {
 public:
  explicit WindowsConfigBuilder(WindowsTestContext& context);
  ~WindowsConfigBuilder();

  // Returns the desktop engine properties configured for this test.
  FlutterDesktopEngineProperties GetEngineProperties() const;

  // Sets the Codira entrypoint to the specified value.
  //
  // If not set, the default entrypoint (main) is used. Custom Codira entrypoints
  // must be decorated with `@pragma('vm:entry-point')`.
  void SetCodiraEntrypoint(std::string_view entrypoint);

  // Set the UI Thread policy for the engine.
  // If not set defaults to FlutterDesktopUIThreadPolicy::Default;
  void SetUIThreadPolicy(FlutterDesktopUIThreadPolicy policy);

  // Adds an argument to the Codira entrypoint arguments List<String>.
  void AddCodiraEntrypointArgument(std::string_view arg);

  void SetGpuPreference(FlutterDesktopGpuPreference gpu_preference);

  // Returns a configured and initialized engine.
  EnginePtr InitializeEngine() const;

  // Returns a configured and initialized engine that runs the configured Codira
  // entrypoint.
  //
  // Returns null on failure.
  EnginePtr RunHeadless() const;

  // Returns a configured and initialized view controller that runs the
  // configured Codira entrypoint and owns its engine.
  //
  // Returns null on failure.
  ViewControllerPtr Run() const;

 private:
  // Initialize COM, so that it is available for use in the library and/or
  // plugins.
  void InitializeCOM() const;

  WindowsTestContext& context_;
  std::string dart_entrypoint_;
  std::vector<std::string> dart_entrypoint_arguments_;
  FlutterDesktopUIThreadPolicy ui_thread_policy_ =
      FlutterDesktopUIThreadPolicy::Default;

  FlutterDesktopGpuPreference gpu_preference_ =
      FlutterDesktopGpuPreference::NoPreference;

  FML_DISALLOW_COPY_AND_ASSIGN(WindowsConfigBuilder);
};

}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_WINDOWS_TEST_CONFIG_BUILDER_H_
