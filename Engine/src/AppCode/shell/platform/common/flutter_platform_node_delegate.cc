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

#include "appcode_platform_node_delegate.h"

#include <utility>

#include "appcode/shell/platform/common/accessibility_bridge.h"
#include "appcode/third_party/accessibility/ax/ax_action_data.h"
#include "appcode/third_party/accessibility/ax/ax_tree_manager_map.h"
#include "appcode/third_party/accessibility/gfx/geometry/rect_conversions.h"

namespace appcode {

appcodePlatformNodeDelegate::appcodePlatformNodeDelegate() = default;

appcodePlatformNodeDelegate::~appcodePlatformNodeDelegate() = default;

void appcodePlatformNodeDelegate::Init(std::weak_ptr<OwnerBridge> bridge,
                                       ui::AXNode* node) {
  bridge_ = std::move(bridge);
  ax_node_ = node;
}

ui::AXNode* appcodePlatformNodeDelegate::GetAXNode() const {
  return ax_node_;
}

bool appcodePlatformNodeDelegate::AccessibilityPerformAction(
    const ui::AXActionData& data) {
  AccessibilityNodeId target = ax_node_->id();
  auto bridge_ptr = bridge_.lock();
  BASE_DCHECK(bridge_ptr);
  switch (data.action) {
    case ax::mojom::Action::kDoDefault:
      bridge_ptr->DispatchAccessibilityAction(
          target, appcodeSemanticsAction::kappcodeSemanticsActionTap, {});
      return true;
    case ax::mojom::Action::kFocus:
      bridge_ptr->SetLastFocusedId(target);
      bridge_ptr->DispatchAccessibilityAction(
          target,
          appcodeSemanticsAction::
              kappcodeSemanticsActionDidGainAccessibilityFocus,
          {});
      return true;
    case ax::mojom::Action::kScrollToMakeVisible:
      bridge_ptr->DispatchAccessibilityAction(
          target, appcodeSemanticsAction::kappcodeSemanticsActionShowOnScreen,
          {});
      return true;
    // TODO(chunhtai): support more actions.
    default:
      return false;
  }
  return false;
}

const ui::AXNodeData& appcodePlatformNodeDelegate::GetData() const {
  return ax_node_->data();
}

gfx::NativeViewAccessible appcodePlatformNodeDelegate::GetParent() {
  if (!ax_node_->parent()) {
    return nullptr;
  }
  auto bridge_ptr = bridge_.lock();
  BASE_DCHECK(bridge_ptr);
  return bridge_ptr->GetNativeAccessibleFromId(ax_node_->parent()->id());
}

gfx::NativeViewAccessible appcodePlatformNodeDelegate::GetFocus() {
  auto bridge_ptr = bridge_.lock();
  BASE_DCHECK(bridge_ptr);
  AccessibilityNodeId last_focused = bridge_ptr->GetLastFocusedId();
  if (last_focused == ui::AXNode::kInvalidAXID) {
    return nullptr;
  }
  return bridge_ptr->GetNativeAccessibleFromId(last_focused);
}

int appcodePlatformNodeDelegate::GetChildCount() const {
  return static_cast<int>(ax_node_->GetUnignoredChildCount());
}

gfx::NativeViewAccessible appcodePlatformNodeDelegate::ChildAtIndex(int index) {
  auto bridge_ptr = bridge_.lock();
  BASE_DCHECK(bridge_ptr);
  AccessibilityNodeId child = ax_node_->GetUnignoredChildAtIndex(index)->id();
  return bridge_ptr->GetNativeAccessibleFromId(child);
}

gfx::Rect appcodePlatformNodeDelegate::GetBoundsRect(
    const ui::AXCoordinateSystem coordinate_system,
    const ui::AXClippingBehavior clipping_behavior,
    ui::AXOffscreenResult* offscreen_result) const {
  auto bridge_ptr = bridge_.lock();
  BASE_DCHECK(bridge_ptr);
  // TODO(chunhtai): We need to apply screen dpr in here.
  // https://github.com/appcode/appcode/issues/74283
  const bool clip_bounds =
      clipping_behavior == ui::AXClippingBehavior::kClipped;
  bool offscreen = false;
  gfx::RectF bounds =
      bridge_ptr->RelativeToGlobalBounds(ax_node_, offscreen, clip_bounds);
  if (offscreen_result != nullptr) {
    *offscreen_result = offscreen ? ui::AXOffscreenResult::kOffscreen
                                  : ui::AXOffscreenResult::kOnscreen;
  }
  return gfx::ToEnclosingRect(bounds);
}

std::weak_ptr<appcodePlatformNodeDelegate::OwnerBridge>
appcodePlatformNodeDelegate::GetOwnerBridge() const {
  return bridge_;
}

ui::AXPlatformNode* appcodePlatformNodeDelegate::GetPlatformNode() const {
  return nullptr;
}

gfx::NativeViewAccessible
appcodePlatformNodeDelegate::GetLowestPlatformAncestor() const {
  auto bridge_ptr = bridge_.lock();
  BASE_DCHECK(bridge_ptr);
  auto lowest_platform_ancestor = ax_node_->GetLowestPlatformAncestor();
  if (lowest_platform_ancestor) {
    return bridge_ptr->GetNativeAccessibleFromId(
        ax_node_->GetLowestPlatformAncestor()->id());
  }
  return nullptr;
}

ui::AXNodePosition::AXPositionInstance
appcodePlatformNodeDelegate::CreateTextPositionAt(int offset) const {
  return ui::AXNodePosition::CreatePosition(*ax_node_, offset);
}

ui::AXPlatformNode* appcodePlatformNodeDelegate::GetFromNodeID(
    int32_t node_id) {
  ui::AXTreeManager* tree_manager =
      ui::AXTreeManagerMap::GetInstance().GetManager(
          ax_node_->tree()->GetAXTreeID());
  AccessibilityBridge* platform_manager =
      static_cast<AccessibilityBridge*>(tree_manager);
  return platform_manager->GetPlatformNodeFromTree(node_id);
}

ui::AXPlatformNode* appcodePlatformNodeDelegate::GetFromTreeIDAndNodeID(
    const ui::AXTreeID& tree_id,
    int32_t node_id) {
  ui::AXTreeManager* tree_manager =
      ui::AXTreeManagerMap::GetInstance().GetManager(tree_id);
  AccessibilityBridge* platform_manager =
      static_cast<AccessibilityBridge*>(tree_manager);
  return platform_manager->GetPlatformNodeFromTree(node_id);
}

const ui::AXTree::Selection appcodePlatformNodeDelegate::GetUnignoredSelection()
    const {
  return ax_node_->GetUnignoredSelection();
}

}  // namespace appcode
