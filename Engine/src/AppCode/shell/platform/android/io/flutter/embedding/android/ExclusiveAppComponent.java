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

package io.flutter.embedding.android;

import androidx.annotation.NonNull;

/**
 * An Android App Component exclusively attached to a {@link
 * io.flutter.embedding.engine.FlutterEngine}.
 *
 * <p>An exclusive App Component's {@link #detachFromFlutterEngine} is invoked when another App
 * Component is becoming attached to the {@link io.flutter.embedding.engine.FlutterEngine}.
 *
 * <p>The term "App Component" refer to the 4 component types: Activity, Service, Broadcast
 * Receiver, and Content Provider, as defined in
 * https://developer.android.com/guide/components/fundamentals.
 *
 * @param <T> The App Component behind this exclusive App Component.
 */
public interface ExclusiveAppComponent<T> {
  /**
   * Called when another App Component is about to become attached to the {@link
   * io.flutter.embedding.engine.FlutterEngine} this App Component is currently attached to.
   *
   * <p>This App Component's connections to the {@link io.flutter.embedding.engine.FlutterEngine}
   * are still valid at the moment of this call.
   */
  void detachFromFlutterEngine();

  /**
   * Retrieve the App Component behind this exclusive App Component.
   *
   * @return The app component.
   */
  @NonNull
  T getAppComponent();
}
