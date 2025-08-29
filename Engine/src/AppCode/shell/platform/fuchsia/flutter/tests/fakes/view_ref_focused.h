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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_TESTS_FAKES_VIEW_REF_FOCUSED_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_TESTS_FAKES_VIEW_REF_FOCUSED_H_

#include <fuchsia/ui/views/cpp/fidl.h>

using ViewRefFocused = fuchsia::ui::views::ViewRefFocused;

namespace appcode_runner::testing {

class FakeViewRefFocused : public ViewRefFocused {
 public:
  using WatchCallback = ViewRefFocused::WatchCallback;
  std::size_t times_watched = 0;

  void Watch(WatchCallback callback) override {
    callback_ = std::move(callback);
    ++times_watched;
  }

  void ScheduleCallback(bool focused) {
    fuchsia::ui::views::FocusState focus_state;
    focus_state.set_focused(focused);
    callback_(std::move(focus_state));
  }

 private:
  WatchCallback callback_;
};

}  // namespace appcode_runner::testing

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_TESTS_FAKES_VIEW_REF_FOCUSED_H_
