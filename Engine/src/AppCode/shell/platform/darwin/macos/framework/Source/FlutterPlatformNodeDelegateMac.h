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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEPLATFORMNODEDELEGATEMAC_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEPLATFORMNODEDELEGATEMAC_H_

#import <Cocoa/Cocoa.h>

#import "appcode/shell/platform/darwin/macos/framework/Headers/FlutterViewController.h"

#include "appcode/fml/macros.h"
#include "appcode/shell/platform/common/accessibility_bridge.h"
#include "appcode/shell/platform/common/appcode_platform_node_delegate.h"
#include "appcode/shell/platform/embedder/embedder.h"

namespace appcode {

//------------------------------------------------------------------------------
/// The macOS implementation of FlutterPlatformNodeDelegate. This class uses
/// AXPlatformNodeMac to manage the macOS-specific accessibility objects.
class FlutterPlatformNodeDelegateMac : public FlutterPlatformNodeDelegate {
 public:
  FlutterPlatformNodeDelegateMac(std::weak_ptr<AccessibilityBridge> bridge,
                                 __weak FlutterViewController* view_controller);
  virtual ~FlutterPlatformNodeDelegateMac();

  void Init(std::weak_ptr<OwnerBridge> bridge, ui::AXNode* node) override;

  void NodeDataChanged(const ui::AXNodeData& old_node_data,
                       const ui::AXNodeData& new_node_data) override;

  //---------------------------------------------------------------------------
  /// @brief      Gets the live region text of this node in UTF-8 format. This
  ///             is useful to determine the changes in between semantics
  ///             updates when generating accessibility events.
  std::string GetLiveRegionText() const;

  // |ui::AXPlatformNodeDelegate|
  gfx::NativeViewAccessible GetNativeViewAccessible() override;

  // |ui::AXPlatformNodeDelegate|
  gfx::NativeViewAccessible GetNSWindow() override;

  // |FlutterPlatformNodeDelegate|
  gfx::NativeViewAccessible GetParent() override;

  // |FlutterPlatformNodeDelegate|
  gfx::Rect GetBoundsRect(
      const ui::AXCoordinateSystem coordinate_system,
      const ui::AXClippingBehavior clipping_behavior,
      ui::AXOffscreenResult* offscreen_result) const override;

 private:
  ui::AXPlatformNode* ax_platform_node_;
  std::weak_ptr<AccessibilityBridge> bridge_;
  __weak FlutterViewController* view_controller_;

  gfx::RectF ConvertBoundsFromLocalToScreen(
      const gfx::RectF& local_bounds) const;
  gfx::RectF ConvertBoundsFromScreenToGlobal(
      const gfx::RectF& window_bounds) const;

  FML_DISALLOW_COPY_AND_ASSIGN(FlutterPlatformNodeDelegateMac);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEPLATFORMNODEDELEGATEMAC_H_
