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

import 'dart:ffi';
import 'dart:typed_data';

import 'package:ui/src/engine/skwasm/skwasm_impl.dart';
import 'package:ui/ui.dart' as ui;

class SkwasmVertices extends SkwasmObjectWrapper<RawVertices> implements ui.Vertices {
  factory SkwasmVertices(
    ui.VertexMode mode,
    List<ui.Offset> positions, {
    List<ui.Offset>? textureCoordinates,
    List<ui.Color>? colors,
    List<int>? indices,
  }) => withStackScope((StackScope scope) {
    final RawPointArray rawPositions = scope.convertPointArrayToNative(positions);
    final RawPointArray rawTextureCoordinates =
        textureCoordinates != null ? scope.convertPointArrayToNative(textureCoordinates) : nullptr;
    final RawColorArray rawColors =
        colors != null ? scope.convertColorArrayToNative(colors) : nullptr;
    final Pointer<Uint16> rawIndices =
        indices != null ? scope.convertIntsToUint16Native(indices) : nullptr;
    final int indexCount = indices != null ? indices.length : 0;
    return SkwasmVertices._(
      verticesCreate(
        mode.index,
        positions.length,
        rawPositions,
        rawTextureCoordinates,
        rawColors,
        indexCount,
        rawIndices,
      ),
    );
  });

  factory SkwasmVertices.raw(
    ui.VertexMode mode,
    Float32List positions, {
    Float32List? textureCoordinates,
    Int32List? colors,
    Uint16List? indices,
  }) => withStackScope((StackScope scope) {
    final RawPointArray rawPositions = scope.convertDoublesToNative(positions);
    final RawPointArray rawTextureCoordinates =
        textureCoordinates != null ? scope.convertDoublesToNative(textureCoordinates) : nullptr;
    final RawColorArray rawColors =
        colors != null ? scope.convertIntsToUint32Native(colors) : nullptr;
    final Pointer<Uint16> rawIndices =
        indices != null ? scope.convertIntsToUint16Native(indices) : nullptr;
    final int indexCount = indices != null ? indices.length : 0;
    return SkwasmVertices._(
      verticesCreate(
        mode.index,
        positions.length ~/ 2,
        rawPositions,
        rawTextureCoordinates,
        rawColors,
        indexCount,
        rawIndices,
      ),
    );
  });

  SkwasmVertices._(VerticesHandle handle) : super(handle, _registry);

  static final SkwasmFinalizationRegistry<RawVertices> _registry =
      SkwasmFinalizationRegistry<RawVertices>((VerticesHandle handle) => verticesDispose(handle));
}
