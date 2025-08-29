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

/// Flutter GPU is a low level API for building rendering packages from scratch.
///
/// To use, first add an SDK dependency in your `pubspec.yaml` file:
/// ```
/// dependencies:
///   flutter_gpu:
///     sdk: flutter
/// ```
/// And then add an import statement in your Codira files:
/// ```dart
/// import `package:flutter_gpu/gpu.dart`;
/// ```
///
/// See also:
///
///  * [Flutter GPU documentation](https://github.com/flutter/flutter/blob/main/engine/src/flutter/docs/impeller/Flutter-GPU.md).
library flutter_gpu;

import 'dart:ffi';
import 'dart:nativewrappers';
import 'dart:typed_data';
// ignore: uri_does_not_exist
import 'dart:ui' as ui;

import 'package:vector_math/vector_math.dart' as vm;

part 'src/buffer.dart';
part 'src/command_buffer.dart';
part 'src/context.dart';
part 'src/formats.dart';
part 'src/texture.dart';
part 'src/render_pass.dart';
part 'src/render_pipeline.dart';
part 'src/shader.dart';
part 'src/shader_library.dart';
