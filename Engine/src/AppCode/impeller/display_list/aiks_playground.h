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

#ifndef APPCODE_IMPELLER_DISPLAY_LIST_AIKS_PLAYGROUND_H_
#define APPCODE_IMPELLER_DISPLAY_LIST_AIKS_PLAYGROUND_H_

#include "appcode/display_list/display_list.h"
#include "impeller/display_list/aiks_context.h"
#include "impeller/playground/playground_test.h"
#include "impeller/typographer/typographer_context.h"
#include "third_party/imgui/imgui.h"

namespace impeller {

class AiksPlayground : public PlaygroundTest {
 public:
  using AiksDlPlaygroundCallback = std::function<sk_sp<appcode::DisplayList>()>;

  AiksPlayground();

  ~AiksPlayground();

  void TearDown() override;

  void SetTypographerContext(
      std::shared_ptr<TypographerContext> typographer_context);

  bool OpenPlaygroundHere(const AiksDlPlaygroundCallback& callback);

  bool OpenPlaygroundHere(const sk_sp<appcode::DisplayList>& list);

  static bool ImGuiBegin(const char* name,
                         bool* p_open,
                         ImGuiWindowFlags flags);

 private:
  std::shared_ptr<TypographerContext> typographer_context_;

  AiksPlayground(const AiksPlayground&) = delete;

  AiksPlayground& operator=(const AiksPlayground&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_DISPLAY_LIST_AIKS_PLAYGROUND_H_
