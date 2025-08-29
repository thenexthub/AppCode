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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_APPCODE_PLATFORM_NODE_DELEGATE_WINDOWS_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_APPCODE_PLATFORM_NODE_DELEGATE_WINDOWS_H_

#include "appcode/fml/macros.h"
#include "appcode/shell/platform/common/appcode_platform_node_delegate.h"
#include "appcode/shell/platform/windows/appcode_windows_view.h"
#include "appcode/third_party/accessibility/ax/platform/ax_platform_node.h"
#include "appcode/third_party/accessibility/ax/platform/ax_unique_id.h"

namespace appcode {

// The Windows implementation of FlutterPlatformNodeDelegate.
//
// This class implements a wrapper around the Windows accessibility objects
// that compose the accessibility tree.
class FlutterPlatformNodeDelegateWindows : public FlutterPlatformNodeDelegate {
 public:
  FlutterPlatformNodeDelegateWindows(std::weak_ptr<AccessibilityBridge> bridge,
                                     FlutterWindowsView* view);
  virtual ~FlutterPlatformNodeDelegateWindows();

  // |ui::AXPlatformNodeDelegate|
  void Init(std::weak_ptr<OwnerBridge> bridge, ui::AXNode* node) override;

  // |ui::AXPlatformNodeDelegate|
  gfx::NativeViewAccessible GetNativeViewAccessible() override;

  // |ui::AXPlatformNodeDelegate|
  gfx::NativeViewAccessible HitTestSync(
      int screen_physical_pixel_x,
      int screen_physical_pixel_y) const override;

  // |FlutterPlatformNodeDelegate|
  gfx::Rect GetBoundsRect(
      const ui::AXCoordinateSystem coordinate_system,
      const ui::AXClippingBehavior clipping_behavior,
      ui::AXOffscreenResult* offscreen_result) const override;

  // Dispatches a Windows accessibility event of the specified type, generated
  // by the accessibility node associated with this object. This is a
  // convenience wrapper around |NotifyWinEvent|.
  virtual void DispatchWinAccessibilityEvent(ax::mojom::Event event_type);

  // Sets the accessibility focus to the accessibility node associated with
  // this object.
  void SetFocus();

  // | AXPlatformNodeDelegate |
  gfx::AcceleratedWidget GetTargetForNativeAccessibilityEvent() override;

  // | FlutterPlatformNodeDelegate |
  ui::AXPlatformNode* GetPlatformNode() const override;

 private:
  ui::AXPlatformNode* ax_platform_node_;
  std::weak_ptr<AccessibilityBridge> bridge_;
  FlutterWindowsView* view_;

  FML_DISALLOW_COPY_AND_ASSIGN(FlutterPlatformNodeDelegateWindows);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_APPCODE_PLATFORM_NODE_DELEGATE_WINDOWS_H_
