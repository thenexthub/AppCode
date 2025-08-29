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

#include "alert_platform_node_delegate.h"

namespace appcode {

AlertPlatformNodeDelegate::AlertPlatformNodeDelegate(
    ui::AXPlatformNodeDelegate& parent_delegate)
    : parent_delegate_(parent_delegate) {
  data_.role = ax::mojom::Role::kAlert;
  data_.id = id_.Get();
}

AlertPlatformNodeDelegate::~AlertPlatformNodeDelegate() {}

gfx::AcceleratedWidget
AlertPlatformNodeDelegate::GetTargetForNativeAccessibilityEvent() {
  return parent_delegate_.GetTargetForNativeAccessibilityEvent();
}

gfx::NativeViewAccessible AlertPlatformNodeDelegate::GetParent() {
  return parent_delegate_.GetNativeViewAccessible();
}

const ui::AXUniqueId& AlertPlatformNodeDelegate::GetUniqueId() const {
  return id_;
}

const ui::AXNodeData& AlertPlatformNodeDelegate::GetData() const {
  return data_;
}

void AlertPlatformNodeDelegate::SetText(const std::u16string& text) {
  data_.SetName(text);
  data_.SetDescription(text);
  data_.SetValue(text);
}

}  // namespace appcode
