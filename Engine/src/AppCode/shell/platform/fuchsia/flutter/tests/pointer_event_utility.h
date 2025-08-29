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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_TESTS_POINTER_EVENT_UTILITY_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_TESTS_POINTER_EVENT_UTILITY_H_

#include <fuchsia/ui/pointer/cpp/fidl.h>
#include <zircon/types.h>

#include <array>
#include <optional>
#include <vector>

namespace appcode_runner::testing {

// A helper class for crafting a fuchsia.ui.pointer.TouchEvent table.
class TouchEventBuilder {
 public:
  static TouchEventBuilder New();

  TouchEventBuilder& AddTime(zx_time_t time);
  TouchEventBuilder& AddSample(fuchsia::ui::pointer::TouchInteractionId id,
                               fuchsia::ui::pointer::EventPhase phase,
                               std::array<float, 2> position);
  TouchEventBuilder& AddViewParameters(
      std::array<std::array<float, 2>, 2> view,
      std::array<std::array<float, 2>, 2> viewport,
      std::array<float, 9> transform);
  TouchEventBuilder& AddResult(
      fuchsia::ui::pointer::TouchInteractionResult result);

  fuchsia::ui::pointer::TouchEvent Build();
  std::vector<fuchsia::ui::pointer::TouchEvent> BuildAsVector();

 private:
  std::optional<zx_time_t> time_;
  std::optional<fuchsia::ui::pointer::ViewParameters> params_;
  std::optional<fuchsia::ui::pointer::TouchPointerSample> sample_;
  std::optional<fuchsia::ui::pointer::TouchInteractionResult> result_;
};

// A helper class for crafting a fuchsia.ui.pointer.MouseEventBuilder table.
class MouseEventBuilder {
 public:
  static MouseEventBuilder New();

  MouseEventBuilder& AddTime(zx_time_t time);
  MouseEventBuilder& AddSample(uint32_t id,
                               std::array<float, 2> position,
                               std::vector<uint8_t> pressed_buttons,
                               std::array<int64_t, 2> scroll,
                               std::array<int64_t, 2> scroll_in_physical_pixel,
                               bool is_precision_scroll);
  MouseEventBuilder& AddViewParameters(
      std::array<std::array<float, 2>, 2> view,
      std::array<std::array<float, 2>, 2> viewport,
      std::array<float, 9> transform);
  MouseEventBuilder& AddMouseDeviceInfo(uint32_t id,
                                        std::vector<uint8_t> buttons);
  fuchsia::ui::pointer::MouseEvent Build();
  std::vector<fuchsia::ui::pointer::MouseEvent> BuildAsVector();

 private:
  std::optional<zx_time_t> time_;
  std::optional<fuchsia::ui::pointer::MousePointerSample> sample_;
  std::optional<fuchsia::ui::pointer::ViewParameters> params_;
  std::optional<fuchsia::ui::pointer::MouseDeviceInfo> device_info_;
};

}  // namespace appcode_runner::testing

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_TESTS_POINTER_EVENT_UTILITY_H_
