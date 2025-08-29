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

package io.flutter.util;

import static io.flutter.Build.API_LEVELS;
import static org.junit.Assert.assertTrue;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

@RunWith(RobolectricTestRunner.class)
public class HandlerCompatTest {
  @Test
  @Config(sdk = API_LEVELS.API_28)
  public void createAsync_createsAnAsyncHandler() {
    Handler handler = Handler.createAsync(Looper.getMainLooper());

    Message message = Message.obtain();
    handler.sendMessageAtTime(message, 0);

    assertTrue(message.isAsynchronous());
  }
}
