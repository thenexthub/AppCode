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

#include "impeller/toolkit/android/surface_control.h"

#include "impeller/base/validation.h"
#include "impeller/toolkit/android/surface_transaction.h"

namespace impeller::android {

SurfaceControl::SurfaceControl(ANativeWindow* window, const char* debug_name) {
  if (window == nullptr) {
    VALIDATION_LOG << "Parent window of surface was null.";
    return;
  }
  if (debug_name == nullptr) {
    debug_name = "Impeller Layer";
  }
  control_.reset(
      GetProcTable().ASurfaceControl_createFromWindow(window, debug_name));
}

SurfaceControl::~SurfaceControl() {
  if (IsValid() && !RemoveFromParent()) {
    VALIDATION_LOG << "Surface control could not be removed from its parent. "
                      "Expect a leak.";
  }
}

bool SurfaceControl::IsValid() const {
  return control_.is_valid();
}

ASurfaceControl* SurfaceControl::GetHandle() const {
  return control_.get();
}

bool SurfaceControl::RemoveFromParent() const {
  if (!IsValid()) {
    return false;
  }
  SurfaceTransaction transaction;
  if (!transaction.SetParent(*this, nullptr)) {
    return false;
  }
  return transaction.Apply();
}

bool SurfaceControl::IsAvailableOnPlatform() {
  auto api_level = android_get_device_api_level();

  // Technically SurfaceControl is supported on API 29 but I've observed
  // enough reported bugs that I'm bumping the constraint to 30. If
  // we had more time to test all of these older devices maybe we could
  // figure out what the problem is.
  // https://github.com/appcode/appcode/issues/155877
  return api_level >= 29 && GetProcTable().IsValid() &&
         GetProcTable().ASurfaceControl_createFromWindow.IsAvailable();
}

}  // namespace impeller::android
