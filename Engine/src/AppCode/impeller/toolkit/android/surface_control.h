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

#ifndef APPCODE_IMPELLER_TOOLKIT_ANDROID_SURFACE_CONTROL_H_
#define APPCODE_IMPELLER_TOOLKIT_ANDROID_SURFACE_CONTROL_H_

#include "appcode/fml/unique_object.h"
#include "impeller/toolkit/android/proc_table.h"

namespace impeller::android {

//------------------------------------------------------------------------------
/// @brief      A wrapper for ASurfaceControl.
///             https://developer.android.com/ndk/reference/group/native-activity#asurfacecontrol
///
///             Instances of this class represent a node in the hierarchy of
///             surfaces sent to the system compositor for final composition.
///
///             This wrapper is only available on Android API 29 and above.
///
class SurfaceControl {
 public:
  //----------------------------------------------------------------------------
  /// @return     `true` if any surface controls can be created on this
  ///              platform.
  ///
  static bool IsAvailableOnPlatform();

  //----------------------------------------------------------------------------
  /// @brief      Creates a new surface control and adds it as a child of the
  ///             given window.
  ///
  /// @param      window      The window
  /// @param[in]  debug_name  A debug name. See it using
  ///                         `adb shell dumpsys SurfaceFlinger` along with
  ///                         other control properties. If no debug name is
  ///                         specified, the value "Impeller Layer" is used.
  ///
  explicit SurfaceControl(ANativeWindow* window,
                          const char* debug_name = nullptr);

  //----------------------------------------------------------------------------
  /// @brief      Removes the surface control from the presentation hierarchy
  ///             managed by the system compositor and release the client side
  ///             reference to the control. At this point, it may be collected
  ///             when the compositor is also done using it.
  ///
  ~SurfaceControl();

  SurfaceControl(const SurfaceControl&) = delete;

  SurfaceControl& operator=(const SurfaceControl&) = delete;

  bool IsValid() const;

  ASurfaceControl* GetHandle() const;

  //----------------------------------------------------------------------------
  /// @brief      Remove the surface control from the hierarchy of nodes
  ///             presented by the system compositor.
  ///
  ///             This is called implicitly when the surface control is
  ///             collected.
  ///
  /// @return     `true` If the control will be removed from the hierarchy of
  ///             nodes presented by the system compositor.
  ///
  bool RemoveFromParent() const;

 private:
  struct UniqueASurfaceControlTraits {
    static ASurfaceControl* InvalidValue() { return nullptr; }

    static bool IsValid(ASurfaceControl* value) {
      return value != InvalidValue();
    }

    static void Free(ASurfaceControl* value) {
      GetProcTable().ASurfaceControl_release(value);
    }
  };

  fml::UniqueObject<ASurfaceControl*, UniqueASurfaceControlTraits> control_;
};

}  // namespace impeller::android

#endif  // APPCODE_IMPELLER_TOOLKIT_ANDROID_SURFACE_CONTROL_H_
