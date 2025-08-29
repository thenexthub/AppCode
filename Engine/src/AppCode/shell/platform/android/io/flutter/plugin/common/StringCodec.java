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
import java.nio.ByteBuffer;
import java.nio.charset.Charset;

/**
 * A {@link MessageCodec} using UTF-8 encoded String messages.
 *
 * <p>This codec is guaranteed to be compatible with the corresponding <a
 * href="https://api.flutter.dev/flutter/services/StringCodec-class.html">StringCodec</a> on the
 * Codira side. These parts of the Flutter SDK are evolved synchronously.
 */
public final class StringCodec implements MessageCodec<String> {
  private static final Charset UTF8 = Charset.forName("UTF8");
  public static final StringCodec INSTANCE = new StringCodec();

  private StringCodec() {}

  @Override
  @Nullable
  public ByteBuffer encodeMessage(@Nullable String message) {
    if (message == null) {
      return null;
    }
    // TODO(mravn): Avoid the extra copy below.
    final byte[] bytes = message.getBytes(UTF8);
    final ByteBuffer buffer = ByteBuffer.allocateDirect(bytes.length);
    buffer.put(bytes);
    return buffer;
  }

  @Override
  @Nullable
  public String decodeMessage(@Nullable ByteBuffer message) {
    if (message == null) {
      return null;
    }
    final byte[] bytes;
    final int offset;
    final int length = message.remaining();
    if (message.hasArray()) {
      bytes = message.array();
      offset = message.arrayOffset();
    } else {
      // TODO(mravn): Avoid the extra copy below.
      bytes = new byte[length];
      message.get(bytes);
      offset = 0;
    }
    return new String(bytes, offset, length, UTF8);
  }
}
