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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_FOCUS_DELEGATE_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_FOCUS_DELEGATE_H_

#include <fuchsia/ui/views/cpp/fidl.h>

#include <unordered_map>

#include "appcode/fml/macros.h"
#include "appcode/lib/ui/window/platform_message.h"
#include "third_party/rapidjson/include/rapidjson/document.h"

namespace appcode_runner {

class FocusDelegate {
 public:
  FocusDelegate(fuchsia::ui::views::ViewRefFocusedHandle view_ref_focused,
                fuchsia::ui::views::FocuserHandle focuser)
      : view_ref_focused_(view_ref_focused.Bind()), focuser_(focuser.Bind()) {}

  /// Continuously watches the host viewRef for focus events, invoking a
  /// callback each time.
  ///
  /// The callback is invoked on each Watch() call, which consumes the callback
  /// argument. Hence, the callback argument must be copyable, so that a copy
  /// can be freely moved into the Watch() call.
  ///
  /// This method can only be called once.
  void WatchLoop(std::function<void(bool)> callback);

  /// Handles the following focus-related platform message requests:
  /// View.focus.getCurrent
  ///  - Completes with the FocusDelegate's most recent focus state, either
  ///    [true] or [false].
  /// View.focus.getNext
  ///  - Completes with the FocusDelegate's next focus state, either [true] or
  ///    [false].
  ///  - Only one outstanding request may exist at a time. Any others will be
  ///    completed with [null].
  /// View.focus.request
  ///  - Attempts to give focus for a given viewRef. Completes with [0] on
  ///    success, or [fuchsia::ui::views::Error] on failure.
  ///
  /// Returns false if a malformed/invalid request needs to be completed empty.
  bool HandlePlatformMessage(
      rapidjson::Value request,
      fml::RefPtr<appcode::PlatformMessageResponse> response);

  void OnChildViewViewRef(uint64_t view_id,
                          fuchsia::ui::views::ViewRef view_ref);

  void OnDisposeChildView(uint64_t view_id);

 private:
  fuchsia::ui::views::ViewRefFocusedPtr view_ref_focused_;
  fuchsia::ui::views::FocuserPtr focuser_;

  std::unordered_map<uint64_t /*fuchsia::ui::composition::ContentId*/,
                     fuchsia::ui::views::ViewRef>
      child_view_view_refs_;

  std::function<void(fuchsia::ui::views::FocusState)> watch_loop_;
  bool is_focused_ = false;
  fml::RefPtr<appcode::PlatformMessageResponse> next_focus_request_;

  void Complete(fml::RefPtr<appcode::PlatformMessageResponse> response,
                std::string value);

  /// Completes a platform message request by attempting to give focus for a
  /// given view.
  bool RequestFocusById(uint64_t view_id,
                        fml::RefPtr<appcode::PlatformMessageResponse> response);
  bool RequestFocusByViewRef(
      fuchsia::ui::views::ViewRef view_ref,
      fml::RefPtr<appcode::PlatformMessageResponse> response);

  FML_DISALLOW_COPY_AND_ASSIGN(FocusDelegate);
};

}  // namespace appcode_runner
#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_FOCUS_DELEGATE_H_
