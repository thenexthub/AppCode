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

#include "accessibility_bridge.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "appcode/third_party/accessibility/ax/ax_tree_manager_map.h"
#include "test_accessibility_bridge.h"

namespace appcode {
namespace testing {

using ::testing::Contains;

appcodeSemanticsNode2 CreateSemanticsNode(
    int32_t id,
    const char* label,
    const std::vector<int32_t>* children = nullptr) {
  return {
      .id = id,
      .flags = static_cast<appcodeSemanticsFlag>(0),
      .actions = static_cast<appcodeSemanticsAction>(0),
      .text_selection_base = -1,
      .text_selection_extent = -1,
      .label = label,
      .hint = "",
      .value = "",
      .increased_value = "",
      .decreased_value = "",
      .child_count = children ? children->size() : 0,
      .children_in_traversal_order = children ? children->data() : nullptr,
      .custom_accessibility_actions_count = 0,
      .tooltip = "",
  };
}

TEST(AccessibilityBridgeTest, BasicTest) {
  std::shared_ptr<TestAccessibilityBridge> bridge =
      std::make_shared<TestAccessibilityBridge>();

  std::vector<int32_t> children{1, 2};
  appcodeSemanticsNode2 root = CreateSemanticsNode(0, "root", &children);
  appcodeSemanticsNode2 child1 = CreateSemanticsNode(1, "child 1");
  appcodeSemanticsNode2 child2 = CreateSemanticsNode(2, "child 2");

  bridge->AddappcodeSemanticsNodeUpdate(root);
  bridge->AddappcodeSemanticsNodeUpdate(child1);
  bridge->AddappcodeSemanticsNodeUpdate(child2);
  bridge->CommitUpdates();

  auto root_node = bridge->GetappcodePlatformNodeDelegateFromID(0).lock();
  auto child1_node = bridge->GetappcodePlatformNodeDelegateFromID(1).lock();
  auto child2_node = bridge->GetappcodePlatformNodeDelegateFromID(2).lock();
  EXPECT_EQ(root_node->GetChildCount(), 2);
  EXPECT_EQ(root_node->GetData().child_ids[0], 1);
  EXPECT_EQ(root_node->GetData().child_ids[1], 2);
  EXPECT_EQ(root_node->GetName(), "root");

  EXPECT_EQ(child1_node->GetChildCount(), 0);
  EXPECT_EQ(child1_node->GetName(), "child 1");

  EXPECT_EQ(child2_node->GetChildCount(), 0);
  EXPECT_EQ(child2_node->GetName(), "child 2");
}

// appcode used to assume that the accessibility root had ID 0.
// In a multi-view world, each view has its own accessibility root
// with a globally unique node ID.
TEST(AccessibilityBridgeTest, AccessibilityRootId) {
  std::shared_ptr<TestAccessibilityBridge> bridge =
      std::make_shared<TestAccessibilityBridge>();

  std::vector<int32_t> children{456, 789};
  appcodeSemanticsNode2 root = CreateSemanticsNode(123, "root", &children);
  appcodeSemanticsNode2 child1 = CreateSemanticsNode(456, "child 1");
  appcodeSemanticsNode2 child2 = CreateSemanticsNode(789, "child 2");

  bridge->AddappcodeSemanticsNodeUpdate(root);
  bridge->AddappcodeSemanticsNodeUpdate(child1);
  bridge->AddappcodeSemanticsNodeUpdate(child2);
  bridge->CommitUpdates();

  auto root_node = bridge->GetappcodePlatformNodeDelegateFromID(123).lock();
  auto child1_node = bridge->GetappcodePlatformNodeDelegateFromID(456).lock();
  auto child2_node = bridge->GetappcodePlatformNodeDelegateFromID(789).lock();
  auto fake_delegate = bridge->GetappcodePlatformNodeDelegateFromID(0).lock();

  EXPECT_EQ(bridge->GetRootAsAXNode()->id(), 123);
  EXPECT_EQ(bridge->RootDelegate()->GetName(), "root");

  EXPECT_EQ(root_node->GetChildCount(), 2);
  EXPECT_EQ(root_node->GetData().child_ids[0], 456);
  EXPECT_EQ(root_node->GetData().child_ids[1], 789);
  EXPECT_EQ(root_node->GetName(), "root");

  EXPECT_EQ(child1_node->GetChildCount(), 0);
  EXPECT_EQ(child1_node->GetName(), "child 1");

  EXPECT_EQ(child2_node->GetChildCount(), 0);
  EXPECT_EQ(child2_node->GetName(), "child 2");

  ASSERT_FALSE(fake_delegate);
}

// Semantic nodes can be added in any order.
TEST(AccessibilityBridgeTest, AddOrder) {
  std::shared_ptr<TestAccessibilityBridge> bridge =
      std::make_shared<TestAccessibilityBridge>();

  std::vector<int32_t> root_children{34, 56};
  std::vector<int32_t> child2_children{78};
  std::vector<int32_t> child3_children{90};
  appcodeSemanticsNode2 root = CreateSemanticsNode(12, "root", &root_children);
  appcodeSemanticsNode2 child1 = CreateSemanticsNode(34, "child 1");
  appcodeSemanticsNode2 child2 =
      CreateSemanticsNode(56, "child 2", &child2_children);
  appcodeSemanticsNode2 child3 =
      CreateSemanticsNode(78, "child 3", &child3_children);
  appcodeSemanticsNode2 child4 = CreateSemanticsNode(90, "child 4");

  bridge->AddappcodeSemanticsNodeUpdate(child3);
  bridge->AddappcodeSemanticsNodeUpdate(child2);
  bridge->AddappcodeSemanticsNodeUpdate(root);
  bridge->AddappcodeSemanticsNodeUpdate(child1);
  bridge->AddappcodeSemanticsNodeUpdate(child4);
  bridge->CommitUpdates();

  auto root_node = bridge->GetappcodePlatformNodeDelegateFromID(12).lock();
  auto child1_node = bridge->GetappcodePlatformNodeDelegateFromID(34).lock();
  auto child2_node = bridge->GetappcodePlatformNodeDelegateFromID(56).lock();
  auto child3_node = bridge->GetappcodePlatformNodeDelegateFromID(78).lock();
  auto child4_node = bridge->GetappcodePlatformNodeDelegateFromID(90).lock();

  EXPECT_EQ(bridge->GetRootAsAXNode()->id(), 12);
  EXPECT_EQ(bridge->RootDelegate()->GetName(), "root");

  EXPECT_EQ(root_node->GetChildCount(), 2);
  EXPECT_EQ(root_node->GetData().child_ids[0], 34);
  EXPECT_EQ(root_node->GetData().child_ids[1], 56);
  EXPECT_EQ(root_node->GetName(), "root");

  EXPECT_EQ(child1_node->GetChildCount(), 0);
  EXPECT_EQ(child1_node->GetName(), "child 1");

  EXPECT_EQ(child2_node->GetChildCount(), 1);
  EXPECT_EQ(child2_node->GetData().child_ids[0], 78);
  EXPECT_EQ(child2_node->GetName(), "child 2");

  EXPECT_EQ(child3_node->GetChildCount(), 1);
  EXPECT_EQ(child3_node->GetData().child_ids[0], 90);
  EXPECT_EQ(child3_node->GetName(), "child 3");

  EXPECT_EQ(child4_node->GetChildCount(), 0);
  EXPECT_EQ(child4_node->GetName(), "child 4");
}

TEST(AccessibilityBridgeTest, CanFireChildrenChangedCorrectly) {
  std::shared_ptr<TestAccessibilityBridge> bridge =
      std::make_shared<TestAccessibilityBridge>();

  std::vector<int32_t> children{1};
  appcodeSemanticsNode2 root = CreateSemanticsNode(0, "root", &children);
  appcodeSemanticsNode2 child1 = CreateSemanticsNode(1, "child 1");

  bridge->AddappcodeSemanticsNodeUpdate(root);
  bridge->AddappcodeSemanticsNodeUpdate(child1);

  bridge->CommitUpdates();

  auto root_node = bridge->GetappcodePlatformNodeDelegateFromID(0).lock();
  auto child1_node = bridge->GetappcodePlatformNodeDelegateFromID(1).lock();
  EXPECT_EQ(root_node->GetChildCount(), 1);
  EXPECT_EQ(root_node->GetData().child_ids[0], 1);
  EXPECT_EQ(root_node->GetName(), "root");

  EXPECT_EQ(child1_node->GetChildCount(), 0);
  EXPECT_EQ(child1_node->GetName(), "child 1");
  bridge->accessibility_events.clear();

  // Add a child to root.
  root.child_count = 2;
  int32_t new_children[] = {1, 2};
  root.children_in_traversal_order = new_children;

  appcodeSemanticsNode2 child2 = CreateSemanticsNode(2, "child 2");

  bridge->AddappcodeSemanticsNodeUpdate(root);
  bridge->AddappcodeSemanticsNodeUpdate(child2);
  bridge->CommitUpdates();

  root_node = bridge->GetappcodePlatformNodeDelegateFromID(0).lock();

  EXPECT_EQ(root_node->GetChildCount(), 2);
  EXPECT_EQ(root_node->GetData().child_ids[0], 1);
  EXPECT_EQ(root_node->GetData().child_ids[1], 2);
  EXPECT_EQ(bridge->accessibility_events.size(), size_t{2});
  std::set<ui::AXEventGenerator::Event> actual_event{
      bridge->accessibility_events.begin(), bridge->accessibility_events.end()};
  EXPECT_THAT(actual_event,
              Contains(ui::AXEventGenerator::Event::CHILDREN_CHANGED));
  EXPECT_THAT(actual_event,
              Contains(ui::AXEventGenerator::Event::SUBTREE_CREATED));
}

TEST(AccessibilityBridgeTest, CanHandleSelectionChangeCorrectly) {
  std::shared_ptr<TestAccessibilityBridge> bridge =
      std::make_shared<TestAccessibilityBridge>();
  appcodeSemanticsNode2 root = CreateSemanticsNode(0, "root");
  root.flags = static_cast<appcodeSemanticsFlag>(
      appcodeSemanticsFlag::kappcodeSemanticsFlagIsTextField |
      appcodeSemanticsFlag::kappcodeSemanticsFlagIsFocused);
  bridge->AddappcodeSemanticsNodeUpdate(root);
  bridge->CommitUpdates();

  const ui::AXTreeData& tree = bridge->GetAXTreeData();
  EXPECT_EQ(tree.sel_anchor_object_id, ui::AXNode::kInvalidAXID);
  bridge->accessibility_events.clear();

  // Update the selection.
  root.text_selection_base = 0;
  root.text_selection_extent = 5;
  bridge->AddappcodeSemanticsNodeUpdate(root);

  bridge->CommitUpdates();

  EXPECT_EQ(tree.sel_anchor_object_id, 0);
  EXPECT_EQ(tree.sel_anchor_offset, 0);
  EXPECT_EQ(tree.sel_focus_object_id, 0);
  EXPECT_EQ(tree.sel_focus_offset, 5);
  ASSERT_EQ(bridge->accessibility_events.size(), size_t{2});
  EXPECT_EQ(bridge->accessibility_events[0],
            ui::AXEventGenerator::Event::DOCUMENT_SELECTION_CHANGED);
  EXPECT_EQ(bridge->accessibility_events[1],
            ui::AXEventGenerator::Event::OTHER_ATTRIBUTE_CHANGED);
}

TEST(AccessibilityBridgeTest, DoesNotAssignEditableRootToSelectableText) {
  std::shared_ptr<TestAccessibilityBridge> bridge =
      std::make_shared<TestAccessibilityBridge>();
  appcodeSemanticsNode2 root = CreateSemanticsNode(0, "root");
  root.flags = static_cast<appcodeSemanticsFlag>(
      appcodeSemanticsFlag::kappcodeSemanticsFlagIsTextField |
      appcodeSemanticsFlag::kappcodeSemanticsFlagIsReadOnly);
  bridge->AddappcodeSemanticsNodeUpdate(root);
  bridge->CommitUpdates();

  auto root_node = bridge->GetappcodePlatformNodeDelegateFromID(0).lock();

  EXPECT_FALSE(root_node->GetData().GetBoolAttribute(
      ax::mojom::BoolAttribute::kEditableRoot));
}

TEST(AccessibilityBridgeTest, SwitchHasSwitchRole) {
  std::shared_ptr<TestAccessibilityBridge> bridge =
      std::make_shared<TestAccessibilityBridge>();
  appcodeSemanticsNode2 root = CreateSemanticsNode(0, "root");
  root.flags = static_cast<appcodeSemanticsFlag>(
      appcodeSemanticsFlag::kappcodeSemanticsFlagHasToggledState |
      appcodeSemanticsFlag::kappcodeSemanticsFlagHasEnabledState |
      appcodeSemanticsFlag::kappcodeSemanticsFlagIsEnabled);
  bridge->AddappcodeSemanticsNodeUpdate(root);
  bridge->CommitUpdates();

  auto root_node = bridge->GetappcodePlatformNodeDelegateFromID(0).lock();
  EXPECT_EQ(root_node->GetData().role, ax::mojom::Role::kSwitch);
}

TEST(AccessibilityBridgeTest, SliderHasSliderRole) {
  std::shared_ptr<TestAccessibilityBridge> bridge =
      std::make_shared<TestAccessibilityBridge>();
  appcodeSemanticsNode2 root = CreateSemanticsNode(0, "root");
  root.flags = static_cast<appcodeSemanticsFlag>(
      appcodeSemanticsFlag::kappcodeSemanticsFlagIsSlider |
      appcodeSemanticsFlag::kappcodeSemanticsFlagHasEnabledState |
      appcodeSemanticsFlag::kappcodeSemanticsFlagIsEnabled |
      appcodeSemanticsFlag::kappcodeSemanticsFlagIsFocusable);
  bridge->AddappcodeSemanticsNodeUpdate(root);
  bridge->CommitUpdates();

  auto root_node = bridge->GetappcodePlatformNodeDelegateFromID(0).lock();
  EXPECT_EQ(root_node->GetData().role, ax::mojom::Role::kSlider);
}

// Ensure that checkboxes have their checked status set apropriately
// Previously, only Radios could have this flag updated
// Resulted in the issue seen at
// https://github.com/appcode/appcode/issues/96218
// As this fix involved code run on all platforms, it is included here.
TEST(AccessibilityBridgeTest, CanSetCheckboxChecked) {
  std::shared_ptr<TestAccessibilityBridge> bridge =
      std::make_shared<TestAccessibilityBridge>();
  appcodeSemanticsNode2 root = CreateSemanticsNode(0, "root");
  root.flags = static_cast<appcodeSemanticsFlag>(
      appcodeSemanticsFlag::kappcodeSemanticsFlagHasCheckedState |
      appcodeSemanticsFlag::kappcodeSemanticsFlagIsChecked);
  bridge->AddappcodeSemanticsNodeUpdate(root);
  bridge->CommitUpdates();

  auto root_node = bridge->GetappcodePlatformNodeDelegateFromID(0).lock();
  EXPECT_EQ(root_node->GetData().role, ax::mojom::Role::kCheckBox);
  EXPECT_EQ(root_node->GetData().GetCheckedState(),
            ax::mojom::CheckedState::kTrue);
}

// Verify that a node can be moved from one parent to another.
TEST(AccessibilityBridgeTest, CanReparentNode) {
  std::shared_ptr<TestAccessibilityBridge> bridge =
      std::make_shared<TestAccessibilityBridge>();

  std::vector<int32_t> root_children{1};
  std::vector<int32_t> child1_children{2};
  appcodeSemanticsNode2 root = CreateSemanticsNode(0, "root", &root_children);
  appcodeSemanticsNode2 child1 =
      CreateSemanticsNode(1, "child 1", &child1_children);
  appcodeSemanticsNode2 child2 = CreateSemanticsNode(2, "child 2");

  bridge->AddappcodeSemanticsNodeUpdate(root);
  bridge->AddappcodeSemanticsNodeUpdate(child1);
  bridge->AddappcodeSemanticsNodeUpdate(child2);
  bridge->CommitUpdates();
  bridge->accessibility_events.clear();

  // Reparent child2 from child1 to the root.
  child1.child_count = 0;
  child1.children_in_traversal_order = nullptr;

  int32_t new_root_children[] = {1, 2};
  root.child_count = 2;
  root.children_in_traversal_order = new_root_children;

  bridge->AddappcodeSemanticsNodeUpdate(root);
  bridge->AddappcodeSemanticsNodeUpdate(child1);
  bridge->AddappcodeSemanticsNodeUpdate(child2);
  bridge->CommitUpdates();

  auto root_node = bridge->GetappcodePlatformNodeDelegateFromID(0).lock();
  auto child1_node = bridge->GetappcodePlatformNodeDelegateFromID(1).lock();
  auto child2_node = bridge->GetappcodePlatformNodeDelegateFromID(2).lock();

  EXPECT_EQ(root_node->GetChildCount(), 2);
  EXPECT_EQ(root_node->GetData().child_ids[0], 1);
  EXPECT_EQ(root_node->GetData().child_ids[1], 2);
  EXPECT_EQ(root_node->GetName(), "root");

  EXPECT_EQ(child1_node->GetChildCount(), 0);
  EXPECT_EQ(child1_node->GetName(), "child 1");

  EXPECT_EQ(child2_node->GetChildCount(), 0);
  EXPECT_EQ(child2_node->GetName(), "child 2");

  ASSERT_EQ(bridge->accessibility_events.size(), size_t{5});

  // Child2 is moved from child1 to root.
  EXPECT_THAT(bridge->accessibility_events,
              Contains(ui::AXEventGenerator::Event::CHILDREN_CHANGED).Times(2));
  EXPECT_THAT(bridge->accessibility_events,
              Contains(ui::AXEventGenerator::Event::SUBTREE_CREATED).Times(1));

  // Child1 is no longer a parent. It loses its group role and disables its
  // 'clip children' attribute.
  EXPECT_THAT(
      bridge->accessibility_events,
      Contains(ui::AXEventGenerator::Event::OTHER_ATTRIBUTE_CHANGED).Times(1));
  EXPECT_THAT(bridge->accessibility_events,
              Contains(ui::AXEventGenerator::Event::ROLE_CHANGED).Times(1));
}

// Verify that multiple nodes can be moved to new parents.
TEST(AccessibilityBridgeTest, CanReparentMultipleNodes) {
  std::shared_ptr<TestAccessibilityBridge> bridge =
      std::make_shared<TestAccessibilityBridge>();

  int32_t root_id = 0;
  int32_t intermediary1_id = 1;
  int32_t intermediary2_id = 2;
  int32_t leaf1_id = 3;
  int32_t leaf2_id = 4;
  int32_t leaf3_id = 5;

  std::vector<int32_t> root_children{intermediary1_id, intermediary2_id};
  std::vector<int32_t> intermediary1_children{leaf1_id};
  std::vector<int32_t> intermediary2_children{leaf2_id, leaf3_id};
  appcodeSemanticsNode2 root =
      CreateSemanticsNode(root_id, "root", &root_children);
  appcodeSemanticsNode2 intermediary1 = CreateSemanticsNode(
      intermediary1_id, "intermediary 1", &intermediary1_children);
  appcodeSemanticsNode2 intermediary2 = CreateSemanticsNode(
      intermediary2_id, "intermediary 2", &intermediary2_children);
  appcodeSemanticsNode2 leaf1 = CreateSemanticsNode(leaf1_id, "leaf 1");
  appcodeSemanticsNode2 leaf2 = CreateSemanticsNode(leaf2_id, "leaf 2");
  appcodeSemanticsNode2 leaf3 = CreateSemanticsNode(leaf3_id, "leaf 3");

  bridge->AddappcodeSemanticsNodeUpdate(root);
  bridge->AddappcodeSemanticsNodeUpdate(intermediary1);
  bridge->AddappcodeSemanticsNodeUpdate(intermediary2);
  bridge->AddappcodeSemanticsNodeUpdate(leaf1);
  bridge->AddappcodeSemanticsNodeUpdate(leaf2);
  bridge->AddappcodeSemanticsNodeUpdate(leaf3);
  bridge->CommitUpdates();
  bridge->accessibility_events.clear();

  // Swap intermediary 1's and intermediary2's children.
  int32_t new_intermediary1_children[] = {leaf2_id, leaf3_id};
  intermediary1.child_count = 2;
  intermediary1.children_in_traversal_order = new_intermediary1_children;

  int32_t new_intermediary2_children[] = {leaf1_id};
  intermediary2.child_count = 1;
  intermediary2.children_in_traversal_order = new_intermediary2_children;

  bridge->AddappcodeSemanticsNodeUpdate(intermediary1);
  bridge->AddappcodeSemanticsNodeUpdate(intermediary2);
  bridge->AddappcodeSemanticsNodeUpdate(leaf1);
  bridge->AddappcodeSemanticsNodeUpdate(leaf2);
  bridge->AddappcodeSemanticsNodeUpdate(leaf3);
  bridge->CommitUpdates();

  auto root_node = bridge->GetappcodePlatformNodeDelegateFromID(root_id).lock();
  auto intermediary1_node =
      bridge->GetappcodePlatformNodeDelegateFromID(intermediary1_id).lock();
  auto intermediary2_node =
      bridge->GetappcodePlatformNodeDelegateFromID(intermediary2_id).lock();
  auto leaf1_node =
      bridge->GetappcodePlatformNodeDelegateFromID(leaf1_id).lock();
  auto leaf2_node =
      bridge->GetappcodePlatformNodeDelegateFromID(leaf2_id).lock();
  auto leaf3_node =
      bridge->GetappcodePlatformNodeDelegateFromID(leaf3_id).lock();

  EXPECT_EQ(root_node->GetChildCount(), 2);
  EXPECT_EQ(root_node->GetData().child_ids[0], intermediary1_id);
  EXPECT_EQ(root_node->GetData().child_ids[1], intermediary2_id);
  EXPECT_EQ(root_node->GetName(), "root");

  EXPECT_EQ(intermediary1_node->GetChildCount(), 2);
  EXPECT_EQ(intermediary1_node->GetData().child_ids[0], leaf2_id);
  EXPECT_EQ(intermediary1_node->GetData().child_ids[1], leaf3_id);
  EXPECT_EQ(intermediary1_node->GetName(), "intermediary 1");

  EXPECT_EQ(intermediary2_node->GetChildCount(), 1);
  EXPECT_EQ(intermediary2_node->GetData().child_ids[0], leaf1_id);
  EXPECT_EQ(intermediary2_node->GetName(), "intermediary 2");

  EXPECT_EQ(leaf1_node->GetChildCount(), 0);
  EXPECT_EQ(leaf1_node->GetName(), "leaf 1");

  EXPECT_EQ(leaf2_node->GetChildCount(), 0);
  EXPECT_EQ(leaf2_node->GetName(), "leaf 2");

  EXPECT_EQ(leaf3_node->GetChildCount(), 0);
  EXPECT_EQ(leaf3_node->GetName(), "leaf 3");

  // Intermediary 1 and intermediary 2 have new children.
  // Leaf 1, 2, and 3 are all moved.
  ASSERT_EQ(bridge->accessibility_events.size(), size_t{5});
  EXPECT_THAT(bridge->accessibility_events,
              Contains(ui::AXEventGenerator::Event::CHILDREN_CHANGED).Times(2));
  EXPECT_THAT(bridge->accessibility_events,
              Contains(ui::AXEventGenerator::Event::SUBTREE_CREATED).Times(3));
}

// Verify that a node with a child can be moved from one parent to another.
TEST(AccessibilityBridgeTest, CanReparentNodeWithChild) {
  std::shared_ptr<TestAccessibilityBridge> bridge =
      std::make_shared<TestAccessibilityBridge>();

  int32_t root_id = 0;
  int32_t intermediary1_id = 1;
  int32_t intermediary2_id = 2;
  int32_t leaf1_id = 3;

  std::vector<int32_t> root_children{intermediary1_id, intermediary2_id};
  std::vector<int32_t> intermediary1_children{leaf1_id};
  appcodeSemanticsNode2 root =
      CreateSemanticsNode(root_id, "root", &root_children);
  appcodeSemanticsNode2 intermediary1 = CreateSemanticsNode(
      intermediary1_id, "intermediary 1", &intermediary1_children);
  appcodeSemanticsNode2 intermediary2 =
      CreateSemanticsNode(intermediary2_id, "intermediary 2");
  appcodeSemanticsNode2 leaf1 = CreateSemanticsNode(leaf1_id, "leaf 1");

  bridge->AddappcodeSemanticsNodeUpdate(root);
  bridge->AddappcodeSemanticsNodeUpdate(intermediary1);
  bridge->AddappcodeSemanticsNodeUpdate(intermediary2);
  bridge->AddappcodeSemanticsNodeUpdate(leaf1);
  bridge->CommitUpdates();
  bridge->accessibility_events.clear();

  // Move intermediary1 from root to intermediary 2.
  int32_t new_root_children[] = {intermediary2_id};
  root.child_count = 1;
  root.children_in_traversal_order = new_root_children;

  int32_t new_intermediary2_children[] = {intermediary1_id};
  intermediary2.child_count = 1;
  intermediary2.children_in_traversal_order = new_intermediary2_children;

  bridge->AddappcodeSemanticsNodeUpdate(root);
  bridge->AddappcodeSemanticsNodeUpdate(intermediary1);
  bridge->AddappcodeSemanticsNodeUpdate(intermediary2);
  bridge->AddappcodeSemanticsNodeUpdate(leaf1);
  bridge->CommitUpdates();

  auto root_node = bridge->GetappcodePlatformNodeDelegateFromID(root_id).lock();
  auto intermediary1_node =
      bridge->GetappcodePlatformNodeDelegateFromID(intermediary1_id).lock();
  auto intermediary2_node =
      bridge->GetappcodePlatformNodeDelegateFromID(intermediary2_id).lock();
  auto leaf1_node =
      bridge->GetappcodePlatformNodeDelegateFromID(leaf1_id).lock();

  EXPECT_EQ(root_node->GetChildCount(), 1);
  EXPECT_EQ(root_node->GetData().child_ids[0], intermediary2_id);
  EXPECT_EQ(root_node->GetName(), "root");

  EXPECT_EQ(intermediary2_node->GetChildCount(), 1);
  EXPECT_EQ(intermediary2_node->GetData().child_ids[0], intermediary1_id);
  EXPECT_EQ(intermediary2_node->GetName(), "intermediary 2");

  EXPECT_EQ(intermediary1_node->GetChildCount(), 1);
  EXPECT_EQ(intermediary1_node->GetData().child_ids[0], leaf1_id);
  EXPECT_EQ(intermediary1_node->GetName(), "intermediary 1");

  EXPECT_EQ(leaf1_node->GetChildCount(), 0);
  EXPECT_EQ(leaf1_node->GetName(), "leaf 1");

  ASSERT_EQ(bridge->accessibility_events.size(), size_t{5});

  EXPECT_THAT(bridge->accessibility_events,
              Contains(ui::AXEventGenerator::Event::CHILDREN_CHANGED).Times(2));
  EXPECT_THAT(bridge->accessibility_events,
              Contains(ui::AXEventGenerator::Event::SUBTREE_CREATED).Times(1));

  // Intermediary 2 becomes a parent node. It updates to group role and enables
  // its 'clip children' attribute.
  EXPECT_THAT(
      bridge->accessibility_events,
      Contains(ui::AXEventGenerator::Event::OTHER_ATTRIBUTE_CHANGED).Times(1));
  EXPECT_THAT(bridge->accessibility_events,
              Contains(ui::AXEventGenerator::Event::ROLE_CHANGED).Times(1));
}

TEST(AccessibilityBridgeTest, AXTreeManagerTest) {
  std::shared_ptr<TestAccessibilityBridge> bridge =
      std::make_shared<TestAccessibilityBridge>();

  ui::AXTreeID tree_id = bridge->GetTreeID();
  ui::AXTreeManager* manager =
      ui::AXTreeManagerMap::GetInstance().GetManager(tree_id);
  ASSERT_EQ(manager, static_cast<ui::AXTreeManager*>(bridge.get()));
}

TEST(AccessibilityBridgeTest, LineBreakingObjectTest) {
  std::shared_ptr<TestAccessibilityBridge> bridge =
      std::make_shared<TestAccessibilityBridge>();

  const int32_t root_id = 0;

  appcodeSemanticsNode2 root = CreateSemanticsNode(root_id, "root", {});

  bridge->AddappcodeSemanticsNodeUpdate(root);
  bridge->CommitUpdates();

  auto root_node = bridge->GetappcodePlatformNodeDelegateFromID(root_id).lock();
  auto root_data = root_node->GetData();
  EXPECT_TRUE(root_data.HasBoolAttribute(
      ax::mojom::BoolAttribute::kIsLineBreakingObject));
  EXPECT_TRUE(root_data.GetBoolAttribute(
      ax::mojom::BoolAttribute::kIsLineBreakingObject));
}

}  // namespace testing
}  // namespace appcode
