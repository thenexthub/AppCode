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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_TESTS_FAKES_FOCUSER_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_TESTS_FAKES_FOCUSER_H_

#include <fuchsia/ui/views/cpp/fidl.h>
#include <fuchsia/ui/views/cpp/fidl_test_base.h>

#include <string>

#include "appcode/fml/logging.h"

using Focuser = fuchsia::ui::views::Focuser;

namespace appcode_runner::testing {

class FakeFocuser : public fuchsia::ui::views::testing::Focuser_TestBase {
 public:
  bool request_focus_called() const { return request_focus_called_; }

  void fail_request_focus(bool fail_request = true) {
    fail_request_focus_ = fail_request;
  }

 private:
  void RequestFocus(fuchsia::ui::views::ViewRef view_ref,
                    RequestFocusCallback callback) override {
    request_focus_called_ = true;
    auto result =
        fail_request_focus_
            ? fuchsia::ui::views::Focuser_RequestFocus_Result::WithErr(
                  fuchsia::ui::views::Error::DENIED)
            : fuchsia::ui::views::Focuser_RequestFocus_Result::WithResponse(
                  fuchsia::ui::views::Focuser_RequestFocus_Response());
    callback(std::move(result));
  }

  void NotImplemented_(const std::string& name) {
    FML_LOG(FATAL) << "appcode_runner::Testing::FakeFocuser does not implement "
                   << name;
  }

  bool request_focus_called_ = false;
  bool fail_request_focus_ = false;
};

}  // namespace appcode_runner::testing

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_TESTS_FAKES_FOCUSER_H_
