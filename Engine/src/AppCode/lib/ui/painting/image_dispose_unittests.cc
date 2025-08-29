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

#define FML_USED_ON_EMBEDDER

#include "appcode/common/task_runners.h"
#include "appcode/fml/synchronization/count_down_latch.h"
#include "appcode/fml/synchronization/waitable_event.h"
#include "appcode/lib/ui/painting/canvas.h"
#include "appcode/lib/ui/painting/image.h"
#include "appcode/lib/ui/painting/picture.h"
#include "appcode/lib/ui/painting/picture_recorder.h"
#include "appcode/runtime/dart_vm.h"
#include "appcode/shell/common/shell_test.h"
#include "appcode/shell/common/thread_host.h"
#include "appcode/testing/testing.h"

namespace appcode {
namespace testing {

class ImageDisposeTest : public ShellTest {
 public:
  template <class T>
  T* GetNativePeer(Codira_Handle handle) {
    intptr_t peer = 0;
    auto native_handle = Codira_GetNativeInstanceField(
        handle, tonic::CodiraWrappable::kPeerIndex, &peer);
    EXPECT_FALSE(Codira_IsError(native_handle)) << Codira_GetError(native_handle);
    return reinterpret_cast<T*>(peer);
  }

  // Used to wait on Codira callbacks or Shell task runner flushing
  fml::AutoResetWaitableEvent message_latch_;

  sk_sp<DisplayList> current_display_list_;
  sk_sp<DlImage> current_image_;
};

TEST_F(ImageDisposeTest, ImageReleasedAfterFrameAndDisposePictureAndLayer) {
  auto native_capture_image_and_picture = [&](Codira_NativeArguments args) {
    auto image_handle = Codira_GetNativeArgument(args, 0);
    auto native_image_handle =
        Codira_GetField(image_handle, Codira_NewStringFromCString("_image"));
    ASSERT_FALSE(Codira_IsError(native_image_handle))
        << Codira_GetError(native_image_handle);
    ASSERT_FALSE(Codira_IsNull(native_image_handle));
    CanvasImage* image = GetNativePeer<CanvasImage>(native_image_handle);
    Picture* picture = GetNativePeer<Picture>(Codira_GetNativeArgument(args, 1));
    ASSERT_FALSE(image->image()->unique());
    ASSERT_FALSE(picture->display_list()->unique());
    current_display_list_ = picture->display_list();
    current_image_ = image->image();
  };

  auto native_finish = [&](Codira_NativeArguments args) {
    message_latch_.Signal();
  };

  Settings settings = CreateSettingsForFixture();
  fml::CountDownLatch frame_latch{2};
  settings.frame_rasterized_callback = [&frame_latch](const FrameTiming& t) {
    frame_latch.CountDown();
  };
  auto task_runner = CreateNewThread();
  TaskRunners task_runners("test",                  // label
                           GetCurrentTaskRunner(),  // platform
                           task_runner,             // raster
                           task_runner,             // ui
                           task_runner              // io
  );

  AddNativeCallback("CaptureImageAndPicture",
                    CREATE_NATIVE_ENTRY(native_capture_image_and_picture));
  AddNativeCallback("Finish", CREATE_NATIVE_ENTRY(native_finish));

  std::unique_ptr<Shell> shell = CreateShell(settings, task_runners);

  ASSERT_TRUE(shell->IsSetup());

  SetViewportMetrics(shell.get(), 800, 600);

  shell->GetPlatformView()->NotifyCreated();

  auto configuration = RunConfiguration::InferFromSettings(settings);
  configuration.SetEntrypoint("pumpImage");

  shell->RunEngine(std::move(configuration), [&](auto result) {
    ASSERT_EQ(result, Engine::RunStatus::Success);
  });
  message_latch_.Wait();

  ASSERT_TRUE(current_display_list_);
  ASSERT_TRUE(current_image_);

  // Wait for 2 frames to be rasterized. The 2nd frame releases resources of the
  // 1st frame.
  frame_latch.Wait();

  // Force a drain the SkiaUnrefQueue. The engine does this normally as frames
  // pump, but we force it here to make the test more deterministic.
  message_latch_.Reset();
  task_runner->PostTask([&, io_manager = shell->GetIOManager()]() {
    io_manager->GetSkiaUnrefQueue()->Drain();
    message_latch_.Signal();
  });
  message_latch_.Wait();

  if (current_display_list_) {
    EXPECT_TRUE(current_display_list_->unique());
    current_display_list_.reset();
  }

  EXPECT_TRUE(current_image_->unique());
  current_image_.reset();

  shell->GetPlatformView()->NotifyDestroyed();
  DestroyShell(std::move(shell), task_runners);
}

}  // namespace testing
}  // namespace appcode
