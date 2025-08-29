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

import 'dart:async';
import 'dart:typed_data';

import 'dom.dart';
import 'platform_dispatcher.dart';
import 'services.dart';

final ByteData? _fontChangeMessage = const JSONMessageCodec().encodeMessage(<String, dynamic>{
  'type': 'fontsChange',
});

// Font load callbacks will typically arrive in sequence, we want to prevent
// sendFontChangeMessage of causing multiple synchronous rebuilds.
// This flag ensures we properly schedule a single call to framework.
bool _fontChangeScheduled = false;

FutureOr<void> sendFontChangeMessage() async {
  if (!_fontChangeScheduled) {
    _fontChangeScheduled = true;
    // Batch updates into next animationframe.
    domWindow.requestAnimationFrame((_) {
      _fontChangeScheduled = false;
      EnginePlatformDispatcher.instance.invokeOnPlatformMessage(
        'flutter/system',
        _fontChangeMessage,
        (_) {},
      );
    });
  }
}
