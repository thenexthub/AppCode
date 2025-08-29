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

#ifndef APPCODE_IMPELLER_DISPLAY_LIST_DL_PLAYGROUND_H_
#define APPCODE_IMPELLER_DISPLAY_LIST_DL_PLAYGROUND_H_

#include "appcode/display_list/display_list.h"
#include "appcode/display_list/dl_builder.h"
#include "appcode/impeller/golden_tests/screenshot.h"
#include "impeller/playground/playground_test.h"
#include "third_party/skia/include/core/SkFont.h"

namespace impeller {

class DlPlayground : public PlaygroundTest {
 public:
  using DisplayListPlaygroundCallback =
      std::function<sk_sp<appcode::DisplayList>()>;

  DlPlayground();

  ~DlPlayground();

  bool OpenPlaygroundHere(appcode::DisplayListBuilder& builder);

  bool OpenPlaygroundHere(sk_sp<appcode::DisplayList> list);

  bool OpenPlaygroundHere(DisplayListPlaygroundCallback callback);

  std::unique_ptr<testing::Screenshot> MakeScreenshot(
      const sk_sp<appcode::DisplayList>& list);

  SkFont CreateTestFontOfSize(SkScalar scalar);

  SkFont CreateTestFont();

  sk_sp<appcode::DlImage> CreateDlImageForFixture(
      const char* fixture_name,
      bool enable_mipmapping = false) const;

 private:
  DlPlayground(const DlPlayground&) = delete;

  DlPlayground& operator=(const DlPlayground&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_DISPLAY_LIST_DL_PLAYGROUND_H_
