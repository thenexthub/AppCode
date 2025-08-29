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

#include <oleacc.h>

#include "appcode/shell/platform/windows/appcode_platform_node_delegate_windows.h"

#include "appcode/fml/logging.h"
#include "appcode/shell/platform/windows/accessibility_bridge_windows.h"
#include "appcode/shell/platform/windows/appcode_windows_view.h"
#include "appcode/third_party/accessibility/ax/ax_clipping_behavior.h"
#include "appcode/third_party/accessibility/ax/ax_coordinate_system.h"
#include "appcode/third_party/accessibility/ax/platform/ax_fragment_root_win.h"

namespace appcode {

appcodePlatformNodeDelegateWindows::appcodePlatformNodeDelegateWindows(
    std::weak_ptr<AccessibilityBridge> bridge,
    appcodeWindowsView* view)
    : bridge_(bridge), view_(view) {
  FML_DCHECK(!bridge_.expired())
      << "Expired AccessibilityBridge passed to node delegate";
  FML_DCHECK(view_);
}

appcodePlatformNodeDelegateWindows::~appcodePlatformNodeDelegateWindows() {
  if (ax_platform_node_) {
    ax_platform_node_->Destroy();
  }
}

// |ui::AXPlatformNodeDelegate|
void appcodePlatformNodeDelegateWindows::Init(std::weak_ptr<OwnerBridge> bridge,
                                              ui::AXNode* node) {
  appcodePlatformNodeDelegate::Init(bridge, node);
  ax_platform_node_ = ui::AXPlatformNode::Create(this);
  FML_DCHECK(ax_platform_node_) << "Failed to create AXPlatformNode";
}

// |ui::AXPlatformNodeDelegate|
gfx::NativeViewAccessible
appcodePlatformNodeDelegateWindows::GetNativeViewAccessible() {
  FML_DCHECK(ax_platform_node_) << "AXPlatformNode hasn't been created";
  return ax_platform_node_->GetNativeViewAccessible();
}

// |ui::AXPlatformNodeDelegate|
gfx::NativeViewAccessible appcodePlatformNodeDelegateWindows::HitTestSync(
    int screen_physical_pixel_x,
    int screen_physical_pixel_y) const {
  // If this node doesn't contain the point, return.
  ui::AXOffscreenResult result;
  gfx::Rect rect = GetBoundsRect(ui::AXCoordinateSystem::kScreenPhysicalPixels,
                                 ui::AXClippingBehavior::kUnclipped, &result);
  gfx::Point screen_point(screen_physical_pixel_x, screen_physical_pixel_y);
  if (!rect.Contains(screen_point)) {
    return nullptr;
  }

  // If any child in this node's subtree contains the point, return that child.
  auto bridge = bridge_.lock();
  FML_DCHECK(bridge);
  for (const ui::AXNode* child : GetAXNode()->children()) {
    std::shared_ptr<appcodePlatformNodeDelegateWindows> win_delegate =
        std::static_pointer_cast<appcodePlatformNodeDelegateWindows>(
            bridge->GetappcodePlatformNodeDelegateFromID(child->id()).lock());
    FML_DCHECK(win_delegate)
        << "No appcodePlatformNodeDelegate found for node " << child->id();
    auto hit_view = win_delegate->HitTestSync(screen_physical_pixel_x,
                                              screen_physical_pixel_y);
    if (hit_view) {
      return hit_view;
    }
  }

  // If no children contain the point, but this node does, return this node.
  return ax_platform_node_->GetNativeViewAccessible();
}

// |appcodePlatformNodeDelegate|
gfx::Rect appcodePlatformNodeDelegateWindows::GetBoundsRect(
    const ui::AXCoordinateSystem coordinate_system,
    const ui::AXClippingBehavior clipping_behavior,
    ui::AXOffscreenResult* offscreen_result) const {
  gfx::Rect bounds = appcodePlatformNodeDelegate::GetBoundsRect(
      coordinate_system, clipping_behavior, offscreen_result);
  POINT origin{bounds.x(), bounds.y()};
  POINT extent{bounds.x() + bounds.width(), bounds.y() + bounds.height()};
  ClientToScreen(view_->GetWindowHandle(), &origin);
  ClientToScreen(view_->GetWindowHandle(), &extent);
  return gfx::Rect(origin.x, origin.y, extent.x - origin.x,
                   extent.y - origin.y);
}

void appcodePlatformNodeDelegateWindows::DispatchWinAccessibilityEvent(
    ax::mojom::Event event_type) {
  ax_platform_node_->NotifyAccessibilityEvent(event_type);
}

void appcodePlatformNodeDelegateWindows::SetFocus() {
  VARIANT varchild{};
  varchild.vt = VT_I4;
  varchild.lVal = CHILDID_SELF;
  GetNativeViewAccessible()->accSelect(SELFLAG_TAKEFOCUS, varchild);
}

gfx::AcceleratedWidget
appcodePlatformNodeDelegateWindows::GetTargetForNativeAccessibilityEvent() {
  return view_->GetWindowHandle();
}

ui::AXPlatformNode* appcodePlatformNodeDelegateWindows::GetPlatformNode()
    const {
  return ax_platform_node_;
}

}  // namespace appcode
