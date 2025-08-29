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

#ifndef APPCODE_SHELL_COMMON_DISPLAY_MANAGER_H_
#define APPCODE_SHELL_COMMON_DISPLAY_MANAGER_H_

#include <mutex>
#include <vector>

#include "appcode/shell/common/display.h"

namespace appcode {

/// Manages lifecycle of the connected displays. This class is thread-safe.
class DisplayManager {
 public:
  DisplayManager();

  ~DisplayManager();

  /// Returns the display refresh rate of the main display. In cases where there
  /// is only one display connected, it will return that. We do not yet support
  /// cases where there are multiple displays.
  ///
  /// When there are no registered displays, it returns
  /// `kUnknownDisplayRefreshRate`.
  double GetMainDisplayRefreshRate() const;

  /// Handles the display updates.
  void HandleDisplayUpdates(std::vector<std::unique_ptr<Display>> displays);

 private:
  /// Guards `displays_` vector.
  mutable std::mutex displays_mutex_;
  std::vector<std::unique_ptr<Display>> displays_;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_COMMON_DISPLAY_MANAGER_H_
