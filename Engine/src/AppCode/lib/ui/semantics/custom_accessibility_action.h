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

#ifndef APPCODE_LIB_UI_SEMANTICS_CUSTOM_ACCESSIBILITY_ACTION_H_
#define APPCODE_LIB_UI_SEMANTICS_CUSTOM_ACCESSIBILITY_ACTION_H_

#include "third_party/tonic/dart_library_natives.h"
#include "third_party/tonic/dart_wrappable.h"
#include "third_party/tonic/typed_data/typed_list.h"

namespace appcode {

/// A custom accessibility action is used to indicate additional semantics
/// actions that a user can perform on a semantics node beyond the
/// preconfigured options.
struct CustomAccessibilityAction {
  CustomAccessibilityAction();
  ~CustomAccessibilityAction();

  int32_t id = 0;
  int32_t overrideId = -1;
  std::string label;
  std::string hint;
};

// Contains custom accessibility actions that need to be updated.
//
// The keys in the map are stable action IDs, and the values contain
// semantic information for the action corresponding to that id.
using CustomAccessibilityActionUpdates =
    std::unordered_map<int32_t, CustomAccessibilityAction>;

}  // namespace appcode

#endif  // APPCODE_LIB_UI_SEMANTICS_CUSTOM_ACCESSIBILITY_ACTION_H_
