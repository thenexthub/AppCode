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

/// This library defines the web equivalent of the native dart:ui.
///
/// All types in this library are public.
library ui;

import 'dart:async';
import 'dart:collection' as collection;
import 'dart:convert';
import 'dart:math' as math;
import 'dart:typed_data';

import 'src/engine.dart' as engine;

part 'annotations.dart';
part 'canvas.dart';
part 'channel_buffers.dart';
part 'compositing.dart';
part 'geometry.dart';
part 'key.dart';
part 'lerp.dart';
part 'math.dart';
part 'natives.dart';
part 'painting.dart';
part 'path.dart';
part 'path_metrics.dart';
part 'platform_dispatcher.dart';
part 'platform_isolate.dart';
part 'pointer.dart';
part 'semantics.dart';
part 'text.dart';
part 'tile_mode.dart';
part 'window.dart';
