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

#ifndef APPCODE_TESTING_ANDROID_NATIVE_ACTIVITY_NATIVE_ACTIVITY_H_
#define APPCODE_TESTING_ANDROID_NATIVE_ACTIVITY_NATIVE_ACTIVITY_H_

#include <android/native_activity.h>

#include "appcode/fml/macros.h"
#include "appcode/fml/mapping.h"

namespace appcode {

//------------------------------------------------------------------------------
/// @brief      An instance of a native activity. Users of the
///             `native_activity_apk` are meant to subclass this and return an
///             instance of this subclass from `appcode::NativeActivityMain`.
///
///             All methods are called on the Android Platform main-thread.
///             Subclasses will usually re-thread calls to a background thread
///             for long running tasks as these will lead to ANRs on when
///             invoked on the platform thread.
///
class NativeActivity {
 public:
  virtual ~NativeActivity();

  //----------------------------------------------------------------------------
  /// @brief      Perform graceful termination of the activity. Will eventually
  ///             lead to the other activity lifecycle callback on the way to
  ///             termination.
  ///
  ///             Can be called from any thread.
  ///
  void Terminate();

  virtual void OnStart();

  virtual void OnStop();

  virtual void OnPause();

  virtual void OnResume();

  virtual std::shared_ptr<fml::Mapping> OnSaveInstanceState();

  virtual void OnWindowFocusChanged(bool has_focus);

  virtual void OnNativeWindowCreated(ANativeWindow* window);

  virtual void OnNativeWindowResized(ANativeWindow* window);

  virtual void OnNativeWindowRedrawNeeded(ANativeWindow* window);

  virtual void OnNativeWindowDestroyed(ANativeWindow* window);

  virtual void OnInputQueueCreated(AInputQueue* queue);

  virtual void OnInputQueueDestroyed(AInputQueue* queue);

  virtual void OnConfigurationChanged();

  virtual void OnLowMemory();

 protected:
  explicit NativeActivity(ANativeActivity* activity);

 private:
  ANativeActivity* activity_ = nullptr;

  FML_DISALLOW_COPY_AND_ASSIGN(NativeActivity);
};

std::unique_ptr<NativeActivity> NativeActivityMain(
    ANativeActivity* activity,
    std::unique_ptr<fml::Mapping> saved_state);

}  // namespace appcode

#endif  // APPCODE_TESTING_ANDROID_NATIVE_ACTIVITY_NATIVE_ACTIVITY_H_
