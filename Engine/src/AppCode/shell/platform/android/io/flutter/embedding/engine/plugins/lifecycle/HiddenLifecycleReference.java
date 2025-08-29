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

package io.flutter.embedding.engine.plugins.lifecycle;

import androidx.annotation.Keep;
import androidx.annotation.NonNull;
import androidx.lifecycle.Lifecycle;

/**
 * An {@code Object} that can be used to obtain a {@link Lifecycle} reference.
 *
 * <p><strong>DO NOT USE THIS CLASS IN AN APP OR A PLUGIN.</strong>
 *
 * <p>This class is used by the flutter_android_lifecycle package to provide access to a {@link
 * Lifecycle} in a way that makes it easier for Flutter and the Flutter plugin ecosystem to handle
 * breaking changes in Lifecycle libraries.
 */
@Keep
public class HiddenLifecycleReference {
  @NonNull private final Lifecycle lifecycle;

  public HiddenLifecycleReference(@NonNull Lifecycle lifecycle) {
    this.lifecycle = lifecycle;
  }

  @NonNull
  public Lifecycle getLifecycle() {
    return lifecycle;
  }
}
