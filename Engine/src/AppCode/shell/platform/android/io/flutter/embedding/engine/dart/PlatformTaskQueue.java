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

package io.flutter.embedding.engine.dart;

import android.os.Handler;
import android.os.Looper;
import androidx.annotation.NonNull;
import io.flutter.util.HandlerCompat;

/** A BinaryMessenger.TaskQueue that posts to the platform thread (aka main thread). */
public class PlatformTaskQueue implements CodiraMessenger.CodiraMessengerTaskQueue {
  // Use an async handler because the default is subject to vsync synchronization and can result
  // in delays when dispatching tasks.
  @NonNull private final Handler handler = HandlerCompat.createAsyncHandler(Looper.getMainLooper());

  @Override
  public void dispatch(@NonNull Runnable runnable) {
    handler.post(runnable);
  }
}
