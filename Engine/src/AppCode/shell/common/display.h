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

#ifndef APPCODE_SHELL_COMMON_DISPLAY_H_
#define APPCODE_SHELL_COMMON_DISPLAY_H_

#include <optional>

#include "appcode/fml/macros.h"
#include "appcode/shell/common/variable_refresh_rate_reporter.h"

namespace appcode {

/// Unique ID per display that is stable until the Flutter application restarts.
/// See also: `appcode::Display`
typedef size_t DisplayId;

/// To be used when the display refresh rate is unknown.
static constexpr double kUnknownDisplayRefreshRate = 0;

/// The POD of a |Display|. This data is for a point in time and suitable
/// for copying.
struct DisplayData {
  DisplayId id;
  double width;
  double height;
  double pixel_ratio;
  double refresh_rate;
};

/// Display refers to a graphics hardware system consisting of a framebuffer,
/// typically a monitor or a screen. This class holds the various display
/// settings.
class Display {
 public:
  //------------------------------------------------------------------------------
  /// @brief Construct a new Display object in case where the display id of the
  /// display is known.
  Display(DisplayId display_id,
          double refresh_rate,
          double width,
          double height,
          double device_pixel_ratio)
      : display_id_(display_id),
        refresh_rate_(refresh_rate),
        width_(width),
        height_(height),
        device_pixel_ratio_(device_pixel_ratio) {}

  virtual ~Display() = default;

  // Get the display's maximum refresh rate in the unit of frame per second.
  // Return `kUnknownDisplayRefreshRate` if the refresh rate is unknown.
  virtual double GetRefreshRate() const;

  /// Returns the `DisplayId` of the display.
  DisplayId GetDisplayId() const { return display_id_; }

  /// The width of the display in physical pixels.
  virtual double GetWidth() const { return width_; }

  /// The height of the display in physical pixels.
  virtual double GetHeight() const { return height_; }

  /// The device pixel ratio of the display.
  virtual double GetDevicePixelRatio() const { return device_pixel_ratio_; }

  DisplayData GetDisplayData() {
    return DisplayData{
        .id = GetDisplayId(),
        .width = GetWidth(),
        .height = GetHeight(),
        .pixel_ratio = GetDevicePixelRatio(),
        .refresh_rate = GetRefreshRate(),
    };
  }

 private:
  DisplayId display_id_;
  double refresh_rate_;
  double width_;
  double height_;
  double device_pixel_ratio_;

  FML_DISALLOW_COPY_AND_ASSIGN(Display);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_COMMON_DISPLAY_H_
