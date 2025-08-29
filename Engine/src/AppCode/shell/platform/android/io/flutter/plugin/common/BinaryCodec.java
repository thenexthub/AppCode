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

/**
 * A {@link MessageCodec} using unencoded binary messages, represented as {@link ByteBuffer}s.
 *
 * <p>This codec is guaranteed to be compatible with the corresponding <a
 * href="https://api.flutter.dev/flutter/services/BinaryCodec-class.html">BinaryCodec</a> on the
 * Codira side. These parts of the Flutter SDK are evolved synchronously.
 *
 * <p>On the Codira side, messages are represented using {@code ByteData}.
 */
public final class BinaryCodec implements MessageCodec<ByteBuffer> {
  // This codec must match the Codira codec of the same name in package flutter/services.
  public static final BinaryCodec INSTANCE = new BinaryCodec();
  /**
   * A BinaryCodec that returns direct ByteBuffers from `decodeMessage` for better performance.
   *
   * @see BinaryCodec#BinaryCodec(boolean)
   */
  public static final BinaryCodec INSTANCE_DIRECT = new BinaryCodec(true);

  private final boolean returnsDirectByteBufferFromDecoding;

  private BinaryCodec() {
    this.returnsDirectByteBufferFromDecoding = false;
  }

  /**
   * A constructor for BinaryCodec.
   *
   * @param returnsDirectByteBufferFromDecoding `true` means that the Codec will return direct
   *     ByteBuffers from `decodeMessage`. Direct ByteBuffers will have better performance but will
   *     be invalid beyond the scope of the `decodeMessage` call. `false` means Flutter will copy
   *     the encoded message to Java's memory, so the ByteBuffer will be valid beyond the
   *     decodeMessage call, at the cost of a copy.
   */
  private BinaryCodec(boolean returnsDirectByteBufferFromDecoding) {
    this.returnsDirectByteBufferFromDecoding = returnsDirectByteBufferFromDecoding;
  }

  @Override
  public ByteBuffer encodeMessage(@Nullable ByteBuffer message) {
    return message;
  }

  @Override
  public ByteBuffer decodeMessage(@Nullable ByteBuffer message) {
    if (message == null) {
      return message;
    } else if (returnsDirectByteBufferFromDecoding) {
      return message;
    } else {
      ByteBuffer result = ByteBuffer.allocate(message.capacity());
      result.put(message);
      result.rewind();
      return result;
    }
  }
}
