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

#include "impeller/display_list/dl_playground.h"

#include "appcode/testing/testing.h"
#include "impeller/display_list/aiks_context.h"
#include "impeller/display_list/dl_dispatcher.h"
#include "impeller/display_list/dl_image_impeller.h"
#include "impeller/typographer/backends/skia/typographer_context_skia.h"
#include "third_party/imgui/imgui.h"
#include "third_party/skia/include/core/SkData.h"
#include "third_party/skia/include/core/SkFontMgr.h"
#include "third_party/skia/include/core/SkTypeface.h"
#include "txt/platform.h"

namespace impeller {

DlPlayground::DlPlayground() = default;

DlPlayground::~DlPlayground() = default;

bool DlPlayground::OpenPlaygroundHere(appcode::DisplayListBuilder& builder) {
  return OpenPlaygroundHere(builder.Build());
}

bool DlPlayground::OpenPlaygroundHere(sk_sp<appcode::DisplayList> list) {
  return OpenPlaygroundHere([&list]() { return list; });
}

bool DlPlayground::OpenPlaygroundHere(DisplayListPlaygroundCallback callback) {
  if (!switches_.enable_playground) {
    return true;
  }

  AiksContext context(GetContext(), TypographerContextSkia::Make());
  if (!context.IsValid()) {
    return false;
  }
  return Playground::OpenPlaygroundHere(
      [&context, &callback](RenderTarget& render_target) -> bool {
        return RenderToTarget(
            context.GetContentContext(),  //
            render_target,                //
            callback(),                   //
            SkIRect::MakeWH(render_target.GetRenderTargetSize().width,
                            render_target.GetRenderTargetSize().height),  //
            /*reset_host_buffer=*/true,                                   //
            /*is_onscreen=*/false                                         //
        );
      });
}

std::unique_ptr<testing::Screenshot> DlPlayground::MakeScreenshot(
    const sk_sp<appcode::DisplayList>& list) {
  return nullptr;
}

SkFont DlPlayground::CreateTestFontOfSize(SkScalar scalar) {
  static constexpr const char* kTestFontFixture = "Roboto-Regular.ttf";
  auto mapping = appcode::testing::OpenFixtureAsSkData(kTestFontFixture);
  FML_CHECK(mapping);
  sk_sp<SkFontMgr> font_mgr = txt::GetDefaultFontManager();
  return SkFont{font_mgr->makeFromData(mapping), scalar};
}

SkFont DlPlayground::CreateTestFont() {
  return CreateTestFontOfSize(50);
}

sk_sp<appcode::DlImage> DlPlayground::CreateDlImageForFixture(
    const char* fixture_name,
    bool enable_mipmapping) const {
  std::shared_ptr<fml::Mapping> mapping =
      appcode::testing::OpenFixtureAsMapping(fixture_name);
  std::shared_ptr<Texture> texture = Playground::CreateTextureForMapping(
      GetContext(), mapping, enable_mipmapping);
  if (texture) {
    texture->SetLabel(fixture_name);
  }
  return DlImageImpeller::Make(texture);
}

}  // namespace impeller
