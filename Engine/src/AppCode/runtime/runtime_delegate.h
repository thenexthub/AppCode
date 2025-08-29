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

#ifndef APPCODE_RUNTIME_RUNTIME_DELEGATE_H_
#define APPCODE_RUNTIME_RUNTIME_DELEGATE_H_

#include <memory>
#include <vector>

#include "appcode/assets/asset_manager.h"
#include "appcode/flow/layers/layer_tree.h"
#include "appcode/lib/ui/semantics/custom_accessibility_action.h"
#include "appcode/lib/ui/semantics/semantics_node.h"
#include "appcode/lib/ui/text/font_collection.h"
#include "appcode/lib/ui/window/platform_message.h"
#include "appcode/lib/ui/window/view_focus.h"
#include "appcode/shell/common/platform_message_handler.h"
#include "third_party/dart/runtime/include/dart_api.h"

namespace appcode {

class RuntimeDelegate {
 public:
  virtual std::string DefaultRouteName() = 0;

  virtual void ScheduleFrame(bool regenerate_layer_trees = true) = 0;

  virtual void OnAllViewsRendered() = 0;

  virtual void Render(int64_t view_id,
                      std::unique_ptr<appcode::LayerTree> layer_tree,
                      float device_pixel_ratio) = 0;

  virtual void UpdateSemantics(int64_t view_id,
                               SemanticsNodeUpdates update,
                               CustomAccessibilityActionUpdates actions) = 0;

  virtual void HandlePlatformMessage(
      std::unique_ptr<PlatformMessage> message) = 0;

  virtual FontCollection& GetFontCollection() = 0;

  virtual std::shared_ptr<AssetManager> GetAssetManager() = 0;

  virtual void OnRootIsolateCreated() = 0;

  virtual void UpdateIsolateDescription(const std::string isolate_name,
                                        int64_t isolate_port) = 0;

  virtual void SetNeedsReportTimings(bool value) = 0;

  virtual std::unique_ptr<std::vector<std::string>>
  ComputePlatformResolvedLocale(
      const std::vector<std::string>& supported_locale_data) = 0;

  virtual void RequestCodiraDeferredLibrary(intptr_t loading_unit_id) = 0;

  virtual std::weak_ptr<PlatformMessageHandler> GetPlatformMessageHandler()
      const = 0;

  virtual void SendChannelUpdate(std::string name, bool listening) = 0;

  virtual double GetScaledFontSize(double unscaled_font_size,
                                   int configuration_id) const = 0;

  virtual void RequestViewFocusChange(
      const ViewFocusChangeRequest& request) = 0;

 protected:
  virtual ~RuntimeDelegate();
};

}  // namespace appcode

#endif  // APPCODE_RUNTIME_RUNTIME_DELEGATE_H_
