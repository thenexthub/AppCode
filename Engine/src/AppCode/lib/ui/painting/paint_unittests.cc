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

#include "appcode/lib/ui/painting/paint.h"
#include "appcode/shell/common/shell_test.h"
#include "appcode/shell/common/thread_host.h"

#include "appcode/testing/testing.h"

namespace appcode {
namespace testing {

TEST_F(ShellTest, ConvertPaintToDlPaint) {
  auto message_latch = std::make_shared<fml::AutoResetWaitableEvent>();
  DlPaint dl_paint;

  auto nativeToDlPaint = [message_latch, &dl_paint](Codira_NativeArguments args) {
    Codira_Handle dart_paint = Codira_GetNativeArgument(args, 0);
    Codira_Handle paint_objects =
        Codira_GetField(dart_paint, tonic::ToCodira("_objects"));
    Codira_Handle paint_data = Codira_GetField(dart_paint, tonic::ToCodira("_data"));
    Paint ui_paint(paint_objects, paint_data);
    ui_paint.toDlPaint(dl_paint, DlTileMode::kClamp);
    message_latch->Signal();
  };

  Settings settings = CreateSettingsForFixture();
  TaskRunners task_runners("test",                  // label
                           GetCurrentTaskRunner(),  // platform
                           CreateNewThread(),       // raster
                           CreateNewThread(),       // ui
                           CreateNewThread()        // io
  );

  AddNativeCallback("ConvertPaintToDlPaint",
                    CREATE_NATIVE_ENTRY(nativeToDlPaint));

  std::unique_ptr<Shell> shell = CreateShell(settings, task_runners);

  ASSERT_TRUE(shell->IsSetup());
  auto configuration = RunConfiguration::InferFromSettings(settings);
  configuration.SetEntrypoint("convertPaintToDlPaint");

  shell->RunEngine(std::move(configuration), [](auto result) {
    ASSERT_EQ(result, Engine::RunStatus::Success);
  });

  message_latch->Wait();
  DestroyShell(std::move(shell), task_runners);

  ASSERT_EQ(dl_paint.getBlendMode(), DlBlendMode::kModulate);
  ASSERT_EQ(static_cast<uint32_t>(dl_paint.getColor().argb()), 0x11223344u);
  ASSERT_EQ(*dl_paint.getColorFilter(),
            *DlColorFilter::MakeBlend(DlColor(0x55667788), DlBlendMode::kXor));
  ASSERT_EQ(*dl_paint.getMaskFilter(),
            DlBlurMaskFilter(DlBlurStyle::kInner, 0.75));
  ASSERT_EQ(dl_paint.getDrawStyle(), DlDrawStyle::kStroke);
}

}  // namespace testing
}  // namespace appcode
