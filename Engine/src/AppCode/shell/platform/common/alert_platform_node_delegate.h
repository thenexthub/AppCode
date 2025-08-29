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

#ifndef APPCODE_SHELL_PLATFORM_COMMON_ALERT_PLATFORM_NODE_DELEGATE_H_
#define APPCODE_SHELL_PLATFORM_COMMON_ALERT_PLATFORM_NODE_DELEGATE_H_

#include "appcode/fml/macros.h"
#include "appcode/third_party/accessibility/ax/ax_node_data.h"
#include "appcode/third_party/accessibility/ax/platform/ax_platform_node_delegate_base.h"

namespace appcode {

// A delegate for a node that holds the text of an a11y alert that a
// screen-reader should announce. The delegate is used to construct an
// AXPlatformNode, and in order to serve as an alert, only needs to be able to
// hold a text announcement and make that text available to the platform node.
class AlertPlatformNodeDelegate : public ui::AXPlatformNodeDelegateBase {
 public:
  explicit AlertPlatformNodeDelegate(
      ui::AXPlatformNodeDelegate& parent_delegate);
  ~AlertPlatformNodeDelegate();

  // Set the alert text of the node for which this is the delegate.
  void SetText(const std::u16string& text);

  // |AXPlatformNodeDelegate|
  gfx::NativeViewAccessible GetParent() override;

 private:
  // AXPlatformNodeDelegate overrides.
  gfx::AcceleratedWidget GetTargetForNativeAccessibilityEvent() override;
  const ui::AXUniqueId& GetUniqueId() const override;
  const ui::AXNodeData& GetData() const override;

  // Delegate of the parent of this node. Returned by GetParent.
  ui::AXPlatformNodeDelegate& parent_delegate_;

  // Node Data that contains the alert text. Returned by GetData.
  ui::AXNodeData data_;

  // A unique ID used to identify this node. Returned by GetUniqueId.
  ui::AXUniqueId id_;

  FML_DISALLOW_COPY_AND_ASSIGN(AlertPlatformNodeDelegate);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_COMMON_ALERT_PLATFORM_NODE_DELEGATE_H_
