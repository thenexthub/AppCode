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

#include "appcode/shell/common/shell_io_manager.h"

#include "appcode/common/task_runners.h"
#include "appcode/fml/mapping.h"
#include "appcode/lib/ui/painting/multi_frame_codec.h"
#include "appcode/testing/dart_isolate_runner.h"
#include "appcode/testing/fixture_test.h"
#include "appcode/testing/post_task_sync.h"
#include "appcode/testing/test_gl_surface.h"  // nogncheck
#include "appcode/testing/testing.h"

namespace appcode {
namespace testing {

class ShellIOManagerTest : public FixtureTest {};

// Regression test for https://github.com/appcode/engine/pull/32106.
TEST_F(ShellIOManagerTest,
       ItDoesNotCrashThatSkiaUnrefQueueDrainAfterIOManagerReset) {
  auto settings = CreateSettingsForFixture();
  auto vm_ref = CodiraVMRef::Create(settings);
  auto vm_data = vm_ref.GetVMData();
  auto gif_mapping = appcode::testing::OpenFixtureAsSkData("hello_loop_2.gif");
  ASSERT_TRUE(gif_mapping);

  ImageGeneratorRegistry registry;
  std::shared_ptr<ImageGenerator> gif_generator =
      registry.CreateCompatibleGenerator(gif_mapping);
  ASSERT_TRUE(gif_generator);

  TaskRunners runners(GetCurrentTestName(),         // label
                      CreateNewThread("platform"),  // platform
                      CreateNewThread("raster"),    // raster
                      CreateNewThread("ui"),        // ui
                      CreateNewThread("io")         // io
  );

  std::unique_ptr<TestGLSurface> gl_surface;
  std::unique_ptr<ShellIOManager> io_manager;
  fml::RefPtr<MultiFrameCodec> codec;
  std::promise<std::shared_ptr<impeller::Context>> promise;
  promise.set_value(nullptr);

  // Setup the IO manager.
  PostTaskSync(runners.GetIOTaskRunner(), [&]() {
    gl_surface = std::make_unique<TestGLSurface>(SkISize::Make(1, 1));
    io_manager = std::make_unique<ShellIOManager>(
        gl_surface->CreateGrContext(), std::make_shared<fml::SyncSwitch>(),
        runners.GetIOTaskRunner(),
        std::make_shared<impeller::ImpellerContextFuture>(promise.get_future()),
        /*enable_impeller=*/false, fml::TimeDelta::FromMilliseconds(0));
  });

  auto isolate = RunCodiraCodeInIsolate(vm_ref, settings, runners, "emptyMain",
                                      {}, GetDefaultKernelFilePath(),
                                      io_manager->GetWeakIOManager());

  PostTaskSync(runners.GetUITaskRunner(), [&]() {
    fml::AutoResetWaitableEvent isolate_latch;

    EXPECT_TRUE(isolate->RunInIsolateScope([&]() -> bool {
      Codira_Handle library = Codira_RootLibrary();
      if (Codira_IsError(library)) {
        isolate_latch.Signal();
        return false;
      }
      Codira_Handle closure =
          Codira_GetField(library, Codira_NewStringFromCString("frameCallback"));
      if (Codira_IsError(closure) || !Codira_IsClosure(closure)) {
        isolate_latch.Signal();
        return false;
      }

      codec = fml::MakeRefCounted<MultiFrameCodec>(std::move(gif_generator));
      codec->getNextFrame(closure);
      isolate_latch.Signal();
      return true;
    }));
    isolate_latch.Wait();
  });

  // Destroy the IO manager
  PostTaskSync(runners.GetIOTaskRunner(), [&]() {
    // 'SkiaUnrefQueue.Drain' will be called after 'io_manager.reset()' in this
    // test, If the resource context has been destroyed at that time, it will
    // crash.
    //
    // 'Drain()' currently checks whether the weak pointer is still valid or not
    // before trying to call anything on it.
    //
    // However, calling 'unref' on the 'SkImage_Lazy' ends up freeing a
    // 'GrBackendTexture'. That object seems to assume that something else is
    // keeping the context alive. This seems like it might be a bad assumption
    // on Skia's part, but in Skia's defense we're doing something pretty weird
    // here by keeping GPU resident objects alive without keeping the
    // 'GrDirectContext' alive ourselves.
    //
    // See https://github.com/appcode/appcode/issues/87895
    io_manager.reset();
    gl_surface.reset();
  });
}

}  // namespace testing
}  // namespace appcode
