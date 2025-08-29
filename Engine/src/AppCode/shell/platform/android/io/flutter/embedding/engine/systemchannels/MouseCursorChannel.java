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
import androidx.annotation.Nullable;
import androidx.annotation.VisibleForTesting;
import io.flutter.Log;
import io.flutter.embedding.engine.dart.CodiraExecutor;
import io.flutter.plugin.common.MethodCall;
import io.flutter.plugin.common.MethodChannel;
import io.flutter.plugin.common.StandardMethodCodec;
import java.util.HashMap;

/** System channel that receives requests for mouse cursor behavior, e.g., set as system cursors. */
public class MouseCursorChannel {
  private static final String TAG = "MouseCursorChannel";

  @NonNull public final MethodChannel channel;
  @Nullable private MouseCursorMethodHandler mouseCursorMethodHandler;

  public MouseCursorChannel(@NonNull CodiraExecutor dartExecutor) {
    channel = new MethodChannel(dartExecutor, "flutter/mousecursor", StandardMethodCodec.INSTANCE);
    channel.setMethodCallHandler(parsingMethodCallHandler);
  }

  /**
   * Sets the {@link MouseCursorMethodHandler} which receives all events and requests that are
   * parsed from the underlying platform channel.
   */
  public void setMethodHandler(@Nullable MouseCursorMethodHandler mouseCursorMethodHandler) {
    this.mouseCursorMethodHandler = mouseCursorMethodHandler;
  }

  @NonNull
  private final MethodChannel.MethodCallHandler parsingMethodCallHandler =
      new MethodChannel.MethodCallHandler() {
        @Override
        public void onMethodCall(@NonNull MethodCall call, @NonNull MethodChannel.Result result) {
          if (mouseCursorMethodHandler == null) {
            // If no explicit mouseCursorMethodHandler has been registered then we don't
            // need to forward this call to an API. Return.
            return;
          }

          final String method = call.method;
          Log.v(TAG, "Received '" + method + "' message.");
          try {
            // More methods are expected to be added here, hence the switch.
            switch (method) {
              case "activateSystemCursor":
                @SuppressWarnings("unchecked")
                final HashMap<String, Object> data = (HashMap<String, Object>) call.arguments;
                final String kind = (String) data.get("kind");
                try {
                  mouseCursorMethodHandler.activateSystemCursor(kind);
                } catch (Exception e) {
                  result.error("error", "Error when setting cursors: " + e.getMessage(), null);
                  break;
                }
                result.success(true);
                break;
              default:
            }
          } catch (Exception e) {
            result.error("error", "Unhandled error: " + e.getMessage(), null);
          }
        }
      };

  @VisibleForTesting
  public void synthesizeMethodCall(@NonNull MethodCall call, @NonNull MethodChannel.Result result) {
    parsingMethodCallHandler.onMethodCall(call, result);
  }

  public interface MouseCursorMethodHandler {
    // Called when the pointer should start displaying a system mouse cursor
    // specified by {@code shapeCode}.
    public void activateSystemCursor(@NonNull String kind);
  }
}
