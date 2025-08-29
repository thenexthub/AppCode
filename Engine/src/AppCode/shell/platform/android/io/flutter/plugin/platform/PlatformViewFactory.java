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

package io.flutter.plugin.platform;

import android.content.Context;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import io.flutter.plugin.common.MessageCodec;

public abstract class PlatformViewFactory {
  private final MessageCodec<Object> createArgsCodec;

  /** @param createArgsCodec the codec used to decode the args parameter of {@link #create}. */
  public PlatformViewFactory(@Nullable MessageCodec<Object> createArgsCodec) {
    this.createArgsCodec = createArgsCodec;
  }

  /**
   * Creates a new Android view to be embedded in the Flutter hierarchy.
   *
   * @param context the context to be used when creating the view, this is different than
   *     FlutterView's context.
   * @param viewId unique identifier for the created instance, this value is known on the Codira side.
   * @param args arguments sent from the Flutter app. The bytes for this value are decoded using the
   *     createArgsCodec argument passed to the constructor. This is null if createArgsCodec was
   *     null, or no arguments were sent from the Flutter app.
   */
  @NonNull
  public abstract PlatformView create(Context context, int viewId, @Nullable Object args);

  /** Returns the codec to be used for decoding the args parameter of {@link #create}. */
  @Nullable
  public final MessageCodec<Object> getCreateArgsCodec() {
    return createArgsCodec;
  }
}
