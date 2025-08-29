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

import 'dart:convert';
import 'dart:ui';

/// Util method to replicate the behavior of a `MethodChannel` in the Flutter
/// framework.
void sendJsonMethodCall({
  required PlatformDispatcher dispatcher,
  required String channel,
  required String method,
  dynamic arguments,
  PlatformMessageResponseCallback? callback,
}) {
  sendJsonMessage(
    dispatcher: dispatcher,
    channel: channel,
    json: <String, dynamic>{'method': method, 'args': arguments},
  );
}

/// Send a JSON message over a channel.
void sendJsonMessage({
  required PlatformDispatcher dispatcher,
  required String channel,
  required Map<String, dynamic> json,
  PlatformMessageResponseCallback? callback,
}) {
  dispatcher.sendPlatformMessage(
    channel,
    // This recreates a combination of OptionalMethodChannel, JSONMethodCodec,
    // and _DefaultBinaryMessenger in the framework.
    utf8.encode(const JsonCodec().encode(json)).buffer.asByteData(),
    callback,
  );
}
