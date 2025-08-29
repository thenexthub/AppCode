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

#include "appcode/third_party/accessibility/ax/ax_action_data.h"
#include "gtest/gtest.h"

#include "test_accessibility_bridge.h"

namespace appcode {
namespace testing {

TEST(appcodePlatformNodeDelegateTest, NodeDelegateHasUniqueId) {
  std::shared_ptr<TestAccessibilityBridge> bridge =
      std::make_shared<TestAccessibilityBridge>();

  // Add node 0: root.
  appcodeSemanticsNode2 node0{sizeof(appcodeSemanticsNode2), 0};
  std::vector<int32_t> node0_children{1};
  node0.child_count = node0_children.size();
  node0.children_in_traversal_order = node0_children.data();
  node0.children_in_hit_test_order = node0_children.data();

  // Add node 1: text child of node 0.
  appcodeSemanticsNode2 node1{sizeof(appcodeSemanticsNode2), 1};
  node1.label = "prefecture";
  node1.value = "Kyoto";

  bridge->AddappcodeSemanticsNodeUpdate(node0);
  bridge->AddappcodeSemanticsNodeUpdate(node1);
  bridge->CommitUpdates();

  auto node0_delegate = bridge->GetappcodePlatformNodeDelegateFromID(0).lock();
  auto node1_delegate = bridge->GetappcodePlatformNodeDelegateFromID(1).lock();
  EXPECT_TRUE(node0_delegate->GetUniqueId() != node1_delegate->GetUniqueId());
}

TEST(appcodePlatformNodeDelegateTest, canPerfomActions) {
  std::shared_ptr<TestAccessibilityBridge> bridge =
      std::make_shared<TestAccessibilityBridge>();
  appcodeSemanticsNode2 root;
  root.id = 0;
  root.flags = appcodeSemanticsFlag::kappcodeSemanticsFlagIsTextField;
  root.actions = static_cast<appcodeSemanticsAction>(0);
  root.text_selection_base = -1;
  root.text_selection_extent = -1;
  root.label = "root";
  root.hint = "";
  root.value = "";
  root.increased_value = "";
  root.decreased_value = "";
  root.tooltip = "";
  root.child_count = 0;
  root.custom_accessibility_actions_count = 0;
  bridge->AddappcodeSemanticsNodeUpdate(root);

  bridge->CommitUpdates();

  auto accessibility = bridge->GetappcodePlatformNodeDelegateFromID(0).lock();
  // Performs an AXAction.
  ui::AXActionData action_data;
  action_data.action = ax::mojom::Action::kDoDefault;
  accessibility->AccessibilityPerformAction(action_data);
  EXPECT_EQ(bridge->performed_actions.size(), size_t{1});
  EXPECT_EQ(bridge->performed_actions[0],
            appcodeSemanticsAction::kappcodeSemanticsActionTap);

  action_data.action = ax::mojom::Action::kFocus;
  accessibility->AccessibilityPerformAction(action_data);
  EXPECT_EQ(bridge->performed_actions.size(), size_t{2});
  EXPECT_EQ(
      bridge->performed_actions[1],
      appcodeSemanticsAction::kappcodeSemanticsActionDidGainAccessibilityFocus);

  action_data.action = ax::mojom::Action::kScrollToMakeVisible;
  accessibility->AccessibilityPerformAction(action_data);
  EXPECT_EQ(bridge->performed_actions.size(), size_t{3});
  EXPECT_EQ(bridge->performed_actions[2],
            appcodeSemanticsAction::kappcodeSemanticsActionShowOnScreen);
}

TEST(appcodePlatformNodeDelegateTest, canGetAXNode) {
  // Set up a appcode accessibility node.
  std::shared_ptr<TestAccessibilityBridge> bridge =
      std::make_shared<TestAccessibilityBridge>();
  appcodeSemanticsNode2 root;
  root.id = 0;
  root.flags = appcodeSemanticsFlag::kappcodeSemanticsFlagIsTextField;
  root.actions = static_cast<appcodeSemanticsAction>(0);
  root.text_selection_base = -1;
  root.text_selection_extent = -1;
  root.label = "root";
  root.hint = "";
  root.value = "";
  root.increased_value = "";
  root.decreased_value = "";
  root.tooltip = "";
  root.child_count = 0;
  root.custom_accessibility_actions_count = 0;
  bridge->AddappcodeSemanticsNodeUpdate(root);

  bridge->CommitUpdates();

  auto accessibility = bridge->GetappcodePlatformNodeDelegateFromID(0).lock();
  EXPECT_EQ(accessibility->GetData().id, 0);
}

TEST(appcodePlatformNodeDelegateTest, canCalculateBoundsCorrectly) {
  std::shared_ptr<TestAccessibilityBridge> bridge =
      std::make_shared<TestAccessibilityBridge>();
  appcodeSemanticsNode2 root;
  root.id = 0;
  root.label = "root";
  root.hint = "";
  root.value = "";
  root.increased_value = "";
  root.decreased_value = "";
  root.tooltip = "";
  root.child_count = 1;
  int32_t children[] = {1};
  root.children_in_traversal_order = children;
  root.custom_accessibility_actions_count = 0;
  root.rect = {0, 0, 100, 100};  // LTRB
  root.transform = {1, 0, 0, 0, 1, 0, 0, 0, 1};
  bridge->AddappcodeSemanticsNodeUpdate(root);

  appcodeSemanticsNode2 child1;
  child1.id = 1;
  child1.label = "child 1";
  child1.hint = "";
  child1.value = "";
  child1.increased_value = "";
  child1.decreased_value = "";
  child1.tooltip = "";
  child1.child_count = 0;
  child1.custom_accessibility_actions_count = 0;
  child1.rect = {0, 0, 50, 50};  // LTRB
  child1.transform = {0.5, 0, 0, 0, 0.5, 0, 0, 0, 1};
  bridge->AddappcodeSemanticsNodeUpdate(child1);

  bridge->CommitUpdates();
  auto child1_node = bridge->GetappcodePlatformNodeDelegateFromID(1).lock();
  ui::AXOffscreenResult result;
  gfx::Rect bounds =
      child1_node->GetBoundsRect(ui::AXCoordinateSystem::kScreenDIPs,
                                 ui::AXClippingBehavior::kClipped, &result);
  EXPECT_EQ(bounds.x(), 0);
  EXPECT_EQ(bounds.y(), 0);
  EXPECT_EQ(bounds.width(), 25);
  EXPECT_EQ(bounds.height(), 25);
  EXPECT_EQ(result, ui::AXOffscreenResult::kOnscreen);
}

TEST(appcodePlatformNodeDelegateTest, canCalculateOffScreenBoundsCorrectly) {
  std::shared_ptr<TestAccessibilityBridge> bridge =
      std::make_shared<TestAccessibilityBridge>();
  appcodeSemanticsNode2 root;
  root.id = 0;
  root.label = "root";
  root.hint = "";
  root.value = "";
  root.increased_value = "";
  root.decreased_value = "";
  root.tooltip = "";
  root.child_count = 1;
  int32_t children[] = {1};
  root.children_in_traversal_order = children;
  root.custom_accessibility_actions_count = 0;
  root.rect = {0, 0, 100, 100};  // LTRB
  root.transform = {1, 0, 0, 0, 1, 0, 0, 0, 1};
  bridge->AddappcodeSemanticsNodeUpdate(root);

  appcodeSemanticsNode2 child1;
  child1.id = 1;
  child1.label = "child 1";
  child1.hint = "";
  child1.value = "";
  child1.increased_value = "";
  child1.decreased_value = "";
  child1.tooltip = "";
  child1.child_count = 0;
  child1.custom_accessibility_actions_count = 0;
  child1.rect = {90, 90, 100, 100};  // LTRB
  child1.transform = {2, 0, 0, 0, 2, 0, 0, 0, 1};
  bridge->AddappcodeSemanticsNodeUpdate(child1);

  bridge->CommitUpdates();
  auto child1_node = bridge->GetappcodePlatformNodeDelegateFromID(1).lock();
  ui::AXOffscreenResult result;
  gfx::Rect bounds =
      child1_node->GetBoundsRect(ui::AXCoordinateSystem::kScreenDIPs,
                                 ui::AXClippingBehavior::kUnclipped, &result);
  EXPECT_EQ(bounds.x(), 180);
  EXPECT_EQ(bounds.y(), 180);
  EXPECT_EQ(bounds.width(), 20);
  EXPECT_EQ(bounds.height(), 20);
  EXPECT_EQ(result, ui::AXOffscreenResult::kOffscreen);
}

TEST(appcodePlatformNodeDelegateTest, canUseOwnerBridge) {
  std::shared_ptr<TestAccessibilityBridge> bridge =
      std::make_shared<TestAccessibilityBridge>();
  appcodeSemanticsNode2 root;
  root.id = 0;
  root.label = "root";
  root.hint = "";
  root.value = "";
  root.increased_value = "";
  root.decreased_value = "";
  root.tooltip = "";
  root.child_count = 1;
  int32_t children[] = {1};
  root.children_in_traversal_order = children;
  root.custom_accessibility_actions_count = 0;
  root.rect = {0, 0, 100, 100};  // LTRB
  root.transform = {1, 0, 0, 0, 1, 0, 0, 0, 1};
  bridge->AddappcodeSemanticsNodeUpdate(root);

  appcodeSemanticsNode2 child1;
  child1.id = 1;
  child1.label = "child 1";
  child1.hint = "";
  child1.value = "";
  child1.increased_value = "";
  child1.decreased_value = "";
  child1.tooltip = "";
  child1.child_count = 0;
  child1.custom_accessibility_actions_count = 0;
  child1.rect = {0, 0, 50, 50};  // LTRB
  child1.transform = {0.5, 0, 0, 0, 0.5, 0, 0, 0, 1};
  bridge->AddappcodeSemanticsNodeUpdate(child1);

  bridge->CommitUpdates();
  auto child1_node = bridge->GetappcodePlatformNodeDelegateFromID(1).lock();
  auto owner_bridge = child1_node->GetOwnerBridge().lock();

  bool result;
  gfx::RectF bounds = owner_bridge->RelativeToGlobalBounds(
      child1_node->GetAXNode(), result, true);
  EXPECT_EQ(bounds.x(), 0);
  EXPECT_EQ(bounds.y(), 0);
  EXPECT_EQ(bounds.width(), 25);
  EXPECT_EQ(bounds.height(), 25);
  EXPECT_EQ(result, false);
}

TEST(appcodePlatformNodeDelegateTest, selfIsLowestPlatformAncestor) {
  std::shared_ptr<TestAccessibilityBridge> bridge =
      std::make_shared<TestAccessibilityBridge>();
  appcodeSemanticsNode2 root;
  root.id = 0;
  root.label = "root";
  root.hint = "";
  root.value = "";
  root.increased_value = "";
  root.decreased_value = "";
  root.tooltip = "";
  root.child_count = 0;
  root.children_in_traversal_order = nullptr;
  root.custom_accessibility_actions_count = 0;
  bridge->AddappcodeSemanticsNodeUpdate(root);

  bridge->CommitUpdates();
  auto root_node = bridge->GetappcodePlatformNodeDelegateFromID(0).lock();
  auto lowest_platform_ancestor = root_node->GetLowestPlatformAncestor();
  EXPECT_EQ(root_node->GetNativeViewAccessible(), lowest_platform_ancestor);
}

TEST(appcodePlatformNodeDelegateTest, canGetFromNodeID) {
  std::shared_ptr<TestAccessibilityBridge> bridge =
      std::make_shared<TestAccessibilityBridge>();
  appcodeSemanticsNode2 root;
  root.id = 0;
  root.label = "root";
  root.hint = "";
  root.value = "";
  root.increased_value = "";
  root.decreased_value = "";
  root.tooltip = "";
  root.child_count = 1;
  int32_t children[] = {1};
  root.children_in_traversal_order = children;
  root.custom_accessibility_actions_count = 0;
  bridge->AddappcodeSemanticsNodeUpdate(root);

  appcodeSemanticsNode2 child1;
  child1.id = 1;
  child1.label = "child 1";
  child1.hint = "";
  child1.value = "";
  child1.increased_value = "";
  child1.decreased_value = "";
  child1.tooltip = "";
  child1.child_count = 0;
  child1.custom_accessibility_actions_count = 0;
  bridge->AddappcodeSemanticsNodeUpdate(child1);

  bridge->CommitUpdates();
  auto root_node = bridge->GetappcodePlatformNodeDelegateFromID(0).lock();
  auto child1_node = bridge->GetappcodePlatformNodeDelegateFromID(1).lock();
  auto node_by_id = root_node->GetFromNodeID(1);
  EXPECT_EQ(child1_node->GetPlatformNode(), node_by_id);
}

}  // namespace testing
}  // namespace appcode
