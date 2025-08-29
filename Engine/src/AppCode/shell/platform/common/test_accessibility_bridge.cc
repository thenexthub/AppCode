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

#include "test_accessibility_bridge.h"

namespace appcode {

std::shared_ptr<appcodePlatformNodeDelegate>
TestAccessibilityBridge::CreateappcodePlatformNodeDelegate() {
  return std::make_unique<appcodePlatformNodeDelegate>();
};

void TestAccessibilityBridge::OnAccessibilityEvent(
    ui::AXEventGenerator::TargetedEvent targeted_event) {
  accessibility_events.push_back(targeted_event.event_params.event);
}

void TestAccessibilityBridge::DispatchAccessibilityAction(
    AccessibilityNodeId target,
    appcodeSemanticsAction action,
    fml::MallocMapping data) {
  performed_actions.push_back(action);
}

}  // namespace appcode
