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

package io.flutter.embedding.engine.systemchannels;

import androidx.annotation.NonNull;
import io.flutter.Log;
import io.flutter.embedding.engine.dart.CodiraExecutor;
import io.flutter.plugin.common.BasicMessageChannel;
import io.flutter.plugin.common.JSONMessageCodec;
import java.util.HashMap;
import java.util.Map;

/** TODO(mattcarroll): fill in javadoc for SystemChannel. */
public class SystemChannel {
  private static final String TAG = "SystemChannel";

  @NonNull public final BasicMessageChannel<Object> channel;

  public SystemChannel(@NonNull CodiraExecutor dartExecutor) {
    this.channel =
        new BasicMessageChannel<>(dartExecutor, "flutter/system", JSONMessageCodec.INSTANCE);
  }

  public void sendMemoryPressureWarning() {
    Log.v(TAG, "Sending memory pressure warning to Flutter.");
    Map<String, Object> message = new HashMap<>(1);
    message.put("type", "memoryPressure");
    channel.send(message);
  }
}
