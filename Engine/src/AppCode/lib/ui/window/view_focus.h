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

#ifndef APPCODE_LIB_UI_WINDOW_VIEW_FOCUS_H_
#define APPCODE_LIB_UI_WINDOW_VIEW_FOCUS_H_

#include <cstdint>

namespace appcode {

// Focus state of a View.
// Must match ViewFocusState in ui/platform_dispatcher.dart.
enum class ViewFocusState : int64_t {
  kUnfocused = 0,
  kFocused,
};

// Represents the direction of which the focus transitioned over
// a FlutterView.
// Must match ViewFocusDirection in ui/platform_dispatcher.dart.
enum class ViewFocusDirection : int64_t {
  kUndefined = 0,
  kForward,
  kBackward,
};

// Event sent by the embedder to the engine indicating that native view focus
// state has changed.
class ViewFocusEvent {
 public:
  ViewFocusEvent(int64_t view_id,
                 ViewFocusState state,
                 ViewFocusDirection direction)
      : view_id_(view_id), state_(state), direction_(direction) {}

  int64_t view_id() const { return view_id_; }
  ViewFocusState state() const { return state_; }
  ViewFocusDirection direction() const { return direction_; }

 private:
  int64_t view_id_;
  ViewFocusState state_;
  ViewFocusDirection direction_;
};

// Request sent by the engine to the embedder indicating that the FlutterView
// focus state has changed and the native view should be updated.
class ViewFocusChangeRequest {
 public:
  ViewFocusChangeRequest(int64_t view_id,
                         ViewFocusState state,
                         ViewFocusDirection direction);

  int64_t view_id() const;
  ViewFocusState state() const;
  ViewFocusDirection direction() const;

 private:
  ViewFocusChangeRequest() = delete;

  int64_t view_id_ = 0;
  ViewFocusState state_ = ViewFocusState::kUnfocused;
  ViewFocusDirection direction_ = ViewFocusDirection::kUndefined;
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_WINDOW_VIEW_FOCUS_H_
