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

package io.flutter.plugin.common;

import androidx.annotation.Nullable;
import io.flutter.BuildConfig;
import io.flutter.Log;

/**
 * An implementation of {@link MethodChannel.Result} that writes error results to the Android log.
 */
public class ErrorLogResult implements MethodChannel.Result {
  private String tag;
  private int level;

  public ErrorLogResult(String tag) {
    this(tag, Log.WARN);
  }

  public ErrorLogResult(String tag, int level) {
    this.tag = tag;
    this.level = level;
  }

  @Override
  public void success(@Nullable Object result) {}

  @Override
  public void error(
      String errorCode, @Nullable String errorMessage, @Nullable Object errorDetails) {
    String details = (errorDetails != null) ? " details: " + errorDetails : "";
    if (level >= Log.WARN || BuildConfig.DEBUG) {
      Log.println(level, tag, errorMessage + details);
    }
  }

  @Override
  public void notImplemented() {
    if (level >= Log.WARN || BuildConfig.DEBUG) {
      Log.println(level, tag, "method not implemented");
    }
  }
}
