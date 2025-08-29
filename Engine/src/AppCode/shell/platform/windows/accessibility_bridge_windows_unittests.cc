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

#include "appcode/shell/platform/windows/accessibility_bridge_windows.h"

#include <comdef.h>
#include <comutil.h>
#include <oleacc.h>

#include <vector>

#include "appcode/fml/macros.h"
#include "appcode/shell/platform/embedder/embedder.h"
#include "appcode/shell/platform/embedder/test_utils/proc_table_replacement.h"
#include "appcode/shell/platform/windows/appcode_platform_node_delegate_windows.h"
#include "appcode/shell/platform/windows/appcode_windows_engine.h"
#include "appcode/shell/platform/windows/appcode_windows_view.h"
#include "appcode/shell/platform/windows/testing/engine_modifier.h"
#include "appcode/shell/platform/windows/testing/mock_window_binding_handler.h"
#include "appcode/shell/platform/windows/testing/test_keyboard.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace appcode {
namespace testing {

namespace {
using ::testing::NiceMock;

// A structure representing a Win32 MSAA event targeting a specified node.
struct MsaaEvent {
  std::shared_ptr<appcodePlatformNodeDelegateWindows> node_delegate;
  ax::mojom::Event event_type;
};

// Accessibility bridge delegate that captures events dispatched to the OS.
class AccessibilityBridgeWindowsSpy : public AccessibilityBridgeWindows {
 public:
  using AccessibilityBridgeWindows::OnAccessibilityEvent;

  explicit AccessibilityBridgeWindowsSpy(appcodeWindowsEngine* engine,
                                         appcodeWindowsView* view)
      : AccessibilityBridgeWindows(view) {}

  void DispatchWinAccessibilityEvent(
      std::shared_ptr<appcodePlatformNodeDelegateWindows> node_delegate,
      ax::mojom::Event event_type) override {
    dispatched_events_.push_back({node_delegate, event_type});
  }

  void SetFocus(std::shared_ptr<appcodePlatformNodeDelegateWindows>
                    node_delegate) override {
    focused_nodes_.push_back(std::move(node_delegate));
  }

  void ResetRecords() {
    dispatched_events_.clear();
    focused_nodes_.clear();
  }

  const std::vector<MsaaEvent>& dispatched_events() const {
    return dispatched_events_;
  }

  const std::vector<int32_t> focused_nodes() const {
    std::vector<int32_t> ids;
    std::transform(focused_nodes_.begin(), focused_nodes_.end(),
                   std::back_inserter(ids),
                   [](std::shared_ptr<appcodePlatformNodeDelegate> node) {
                     return node->GetAXNode()->id();
                   });
    return ids;
  }

 protected:
  std::weak_ptr<appcodePlatformNodeDelegate> GetFocusedNode() override {
    return focused_nodes_.back();
  }

 private:
  std::vector<MsaaEvent> dispatched_events_;
  std::vector<std::shared_ptr<appcodePlatformNodeDelegate>> focused_nodes_;

  FML_DISALLOW_COPY_AND_ASSIGN(AccessibilityBridgeWindowsSpy);
};

// A appcodeWindowsView whose accessibility bridge is an
// AccessibilityBridgeWindowsSpy.
class appcodeWindowsViewSpy : public appcodeWindowsView {
 public:
  appcodeWindowsViewSpy(appcodeWindowsEngine* engine,
                        std::unique_ptr<WindowBindingHandler> handler)
      : appcodeWindowsView(kImplicitViewId, engine, std::move(handler)) {}

 protected:
  virtual std::shared_ptr<AccessibilityBridgeWindows>
  CreateAccessibilityBridge() override {
    return std::make_shared<AccessibilityBridgeWindowsSpy>(GetEngine(), this);
  }

 private:
  FML_DISALLOW_COPY_AND_ASSIGN(appcodeWindowsViewSpy);
};

// Returns an engine instance configured with dummy project path values, and
// overridden methods for sending platform messages, so that the engine can
// respond as if the framework were connected.
std::unique_ptr<appcodeWindowsEngine> GetTestEngine() {
  appcodeDesktopEngineProperties properties = {};
  properties.assets_path = L"C:\\foo\\appcode_assets";
  properties.icu_data_path = L"C:\\foo\\icudtl.dat";
  properties.aot_library_path = L"C:\\foo\\aot.so";
  appcodeProjectBundle project(properties);
  auto engine = std::make_unique<appcodeWindowsEngine>(project);

  EngineModifier modifier(engine.get());
  modifier.embedder_api().UpdateSemanticsEnabled =
      [](appcode_API_SYMBOL(appcodeEngine) engine, bool enabled) {
        return kSuccess;
      };

  MockEmbedderApiForKeyboard(modifier,
                             std::make_shared<MockKeyResponseController>());

  engine->Run();
  return engine;
}

// Populates the AXTree associated with the specified bridge with test data.
//
//        node0
//        /   \
//    node1    node2
//             /   \
//         node3   node4
//
// node0 and node2 are grouping nodes. node1 and node2 are static text nodes.
// node4 is a static text node with no text, and hence has the "ignored" state.
void PopulateAXTree(std::shared_ptr<AccessibilityBridge> bridge) {
  // Add node 0: root.
  appcodeSemanticsNode2 node0{sizeof(appcodeSemanticsNode2), 0};
  std::vector<int32_t> node0_children{1, 2};
  node0.child_count = node0_children.size();
  node0.children_in_traversal_order = node0_children.data();
  node0.children_in_hit_test_order = node0_children.data();

  // Add node 1: text child of node 0.
  appcodeSemanticsNode2 node1{sizeof(appcodeSemanticsNode2), 1};
  node1.label = "prefecture";
  node1.value = "Kyoto";

  // Add node 2: subtree child of node 0.
  appcodeSemanticsNode2 node2{sizeof(appcodeSemanticsNode2), 2};
  std::vector<int32_t> node2_children{3, 4};
  node2.child_count = node2_children.size();
  node2.children_in_traversal_order = node2_children.data();
  node2.children_in_hit_test_order = node2_children.data();

  // Add node 3: text child of node 2.
  appcodeSemanticsNode2 node3{sizeof(appcodeSemanticsNode2), 3};
  node3.label = "city";
  node3.value = "Uji";

  // Add node 4: text child (with no text) of node 2.
  appcodeSemanticsNode2 node4{sizeof(appcodeSemanticsNode2), 4};

  bridge->AddappcodeSemanticsNodeUpdate(node0);
  bridge->AddappcodeSemanticsNodeUpdate(node1);
  bridge->AddappcodeSemanticsNodeUpdate(node2);
  bridge->AddappcodeSemanticsNodeUpdate(node3);
  bridge->AddappcodeSemanticsNodeUpdate(node4);
  bridge->CommitUpdates();
}

ui::AXNode* AXNodeFromID(std::shared_ptr<AccessibilityBridge> bridge,
                         int32_t id) {
  auto node_delegate = bridge->GetappcodePlatformNodeDelegateFromID(id).lock();
  return node_delegate ? node_delegate->GetAXNode() : nullptr;
}

std::shared_ptr<AccessibilityBridgeWindowsSpy> GetAccessibilityBridgeSpy(
    appcodeWindowsView& view) {
  return std::static_pointer_cast<AccessibilityBridgeWindowsSpy>(
      view.accessibility_bridge().lock());
}

void ExpectWinEventFromAXEvent(int32_t node_id,
                               ui::AXEventGenerator::Event ax_event,
                               ax::mojom::Event expected_event) {
  auto engine = GetTestEngine();
  appcodeWindowsViewSpy view{
      engine.get(), std::make_unique<NiceMock<MockWindowBindingHandler>>()};
  EngineModifier modifier{engine.get()};
  modifier.SetImplicitView(&view);
  view.OnUpdateSemanticsEnabled(true);

  auto bridge = GetAccessibilityBridgeSpy(view);
  PopulateAXTree(bridge);

  bridge->ResetRecords();
  bridge->OnAccessibilityEvent({AXNodeFromID(bridge, node_id),
                                {ax_event, ax::mojom::EventFrom::kNone, {}}});
  ASSERT_EQ(bridge->dispatched_events().size(), 1);
  EXPECT_EQ(bridge->dispatched_events()[0].event_type, expected_event);
}

void ExpectWinEventFromAXEventOnFocusNode(int32_t node_id,
                                          ui::AXEventGenerator::Event ax_event,
                                          ax::mojom::Event expected_event,
                                          int32_t focus_id) {
  auto engine = GetTestEngine();
  appcodeWindowsViewSpy view{
      engine.get(), std::make_unique<NiceMock<MockWindowBindingHandler>>()};
  EngineModifier modifier{engine.get()};
  modifier.SetImplicitView(&view);
  view.OnUpdateSemanticsEnabled(true);

  auto bridge = GetAccessibilityBridgeSpy(view);
  PopulateAXTree(bridge);

  bridge->ResetRecords();
  auto focus_delegate =
      bridge->GetappcodePlatformNodeDelegateFromID(focus_id).lock();
  bridge->SetFocus(std::static_pointer_cast<appcodePlatformNodeDelegateWindows>(
      focus_delegate));
  bridge->OnAccessibilityEvent({AXNodeFromID(bridge, node_id),
                                {ax_event, ax::mojom::EventFrom::kNone, {}}});
  ASSERT_EQ(bridge->dispatched_events().size(), 1);
  EXPECT_EQ(bridge->dispatched_events()[0].event_type, expected_event);
  EXPECT_EQ(bridge->dispatched_events()[0].node_delegate->GetAXNode()->id(),
            focus_id);
}

}  // namespace

TEST(AccessibilityBridgeWindows, GetParent) {
  auto engine = GetTestEngine();
  appcodeWindowsViewSpy view{
      engine.get(), std::make_unique<NiceMock<MockWindowBindingHandler>>()};
  EngineModifier modifier{engine.get()};
  modifier.SetImplicitView(&view);
  view.OnUpdateSemanticsEnabled(true);

  auto bridge = view.accessibility_bridge().lock();
  PopulateAXTree(bridge);

  auto node0_delegate = bridge->GetappcodePlatformNodeDelegateFromID(0).lock();
  auto node1_delegate = bridge->GetappcodePlatformNodeDelegateFromID(1).lock();
  EXPECT_EQ(node0_delegate->GetNativeViewAccessible(),
            node1_delegate->GetParent());
}

TEST(AccessibilityBridgeWindows, GetParentOnRootRetunsNullptr) {
  auto engine = GetTestEngine();
  appcodeWindowsViewSpy view{
      engine.get(), std::make_unique<NiceMock<MockWindowBindingHandler>>()};
  EngineModifier modifier{engine.get()};
  modifier.SetImplicitView(&view);
  view.OnUpdateSemanticsEnabled(true);

  auto bridge = view.accessibility_bridge().lock();
  PopulateAXTree(bridge);

  auto node0_delegate = bridge->GetappcodePlatformNodeDelegateFromID(0).lock();
  ASSERT_TRUE(node0_delegate->GetParent() == nullptr);
}

TEST(AccessibilityBridgeWindows, DispatchAccessibilityAction) {
  auto engine = GetTestEngine();
  appcodeWindowsViewSpy view{
      engine.get(), std::make_unique<NiceMock<MockWindowBindingHandler>>()};
  EngineModifier modifier{engine.get()};
  modifier.SetImplicitView(&view);
  view.OnUpdateSemanticsEnabled(true);

  auto bridge = view.accessibility_bridge().lock();
  PopulateAXTree(bridge);

  appcodeSemanticsAction actual_action = kappcodeSemanticsActionTap;
  modifier.embedder_api().SendSemanticsAction = MOCK_ENGINE_PROC(
      SendSemanticsAction,
      ([&actual_action](appcode_API_SYMBOL(appcodeEngine) engine,
                        const appcodeSendSemanticsActionInfo* info) {
        actual_action = info->action;
        return kSuccess;
      }));

  AccessibilityBridgeWindows delegate(&view);
  delegate.DispatchAccessibilityAction(1, kappcodeSemanticsActionCopy, {});
  EXPECT_EQ(actual_action, kappcodeSemanticsActionCopy);
}

TEST(AccessibilityBridgeWindows, OnAccessibilityEventAlert) {
  ExpectWinEventFromAXEvent(0, ui::AXEventGenerator::Event::ALERT,
                            ax::mojom::Event::kAlert);
}

TEST(AccessibilityBridgeWindows, OnAccessibilityEventChildrenChanged) {
  ExpectWinEventFromAXEvent(0, ui::AXEventGenerator::Event::CHILDREN_CHANGED,
                            ax::mojom::Event::kChildrenChanged);
}

TEST(AccessibilityBridgeWindows, OnAccessibilityEventFocusChanged) {
  auto engine = GetTestEngine();
  appcodeWindowsViewSpy view{
      engine.get(), std::make_unique<NiceMock<MockWindowBindingHandler>>()};
  EngineModifier modifier{engine.get()};
  modifier.SetImplicitView(&view);
  view.OnUpdateSemanticsEnabled(true);

  auto bridge = GetAccessibilityBridgeSpy(view);
  PopulateAXTree(bridge);

  bridge->ResetRecords();
  bridge->OnAccessibilityEvent({AXNodeFromID(bridge, 1),
                                {ui::AXEventGenerator::Event::FOCUS_CHANGED,
                                 ax::mojom::EventFrom::kNone,
                                 {}}});
  ASSERT_EQ(bridge->dispatched_events().size(), 1);
  EXPECT_EQ(bridge->dispatched_events()[0].event_type,
            ax::mojom::Event::kFocus);

  ASSERT_EQ(bridge->focused_nodes().size(), 1);
  EXPECT_EQ(bridge->focused_nodes()[0], 1);
}

TEST(AccessibilityBridgeWindows, OnAccessibilityEventIgnoredChanged) {
  // Static test nodes with no text, hint, or scrollability are ignored.
  ExpectWinEventFromAXEvent(4, ui::AXEventGenerator::Event::IGNORED_CHANGED,
                            ax::mojom::Event::kHide);
}

TEST(AccessibilityBridgeWindows, OnAccessibilityImageAnnotationChanged) {
  ExpectWinEventFromAXEvent(
      1, ui::AXEventGenerator::Event::IMAGE_ANNOTATION_CHANGED,
      ax::mojom::Event::kTextChanged);
}

TEST(AccessibilityBridgeWindows, OnAccessibilityLiveRegionChanged) {
  ExpectWinEventFromAXEvent(1, ui::AXEventGenerator::Event::LIVE_REGION_CHANGED,
                            ax::mojom::Event::kLiveRegionChanged);
}

TEST(AccessibilityBridgeWindows, OnAccessibilityNameChanged) {
  ExpectWinEventFromAXEvent(1, ui::AXEventGenerator::Event::NAME_CHANGED,
                            ax::mojom::Event::kTextChanged);
}

TEST(AccessibilityBridgeWindows, OnAccessibilityHScrollPosChanged) {
  ExpectWinEventFromAXEvent(
      1, ui::AXEventGenerator::Event::SCROLL_HORIZONTAL_POSITION_CHANGED,
      ax::mojom::Event::kScrollPositionChanged);
}

TEST(AccessibilityBridgeWindows, OnAccessibilityVScrollPosChanged) {
  ExpectWinEventFromAXEvent(
      1, ui::AXEventGenerator::Event::SCROLL_VERTICAL_POSITION_CHANGED,
      ax::mojom::Event::kScrollPositionChanged);
}

TEST(AccessibilityBridgeWindows, OnAccessibilitySelectedChanged) {
  ExpectWinEventFromAXEvent(1, ui::AXEventGenerator::Event::SELECTED_CHANGED,
                            ax::mojom::Event::kValueChanged);
}

TEST(AccessibilityBridgeWindows, OnAccessibilitySelectedChildrenChanged) {
  ExpectWinEventFromAXEvent(
      2, ui::AXEventGenerator::Event::SELECTED_CHILDREN_CHANGED,
      ax::mojom::Event::kSelectedChildrenChanged);
}

TEST(AccessibilityBridgeWindows, OnAccessibilitySubtreeCreated) {
  ExpectWinEventFromAXEvent(0, ui::AXEventGenerator::Event::SUBTREE_CREATED,
                            ax::mojom::Event::kShow);
}

TEST(AccessibilityBridgeWindows, OnAccessibilityValueChanged) {
  ExpectWinEventFromAXEvent(1, ui::AXEventGenerator::Event::VALUE_CHANGED,
                            ax::mojom::Event::kValueChanged);
}

TEST(AccessibilityBridgeWindows, OnAccessibilityStateChanged) {
  ExpectWinEventFromAXEvent(
      1, ui::AXEventGenerator::Event::WIN_IACCESSIBLE_STATE_CHANGED,
      ax::mojom::Event::kStateChanged);
}

TEST(AccessibilityBridgeWindows, OnDocumentSelectionChanged) {
  ExpectWinEventFromAXEventOnFocusNode(
      1, ui::AXEventGenerator::Event::DOCUMENT_SELECTION_CHANGED,
      ax::mojom::Event::kDocumentSelectionChanged, 2);
}

}  // namespace testing
}  // namespace appcode
