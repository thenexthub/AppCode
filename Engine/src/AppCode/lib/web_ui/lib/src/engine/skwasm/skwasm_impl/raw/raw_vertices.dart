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

@DefaultAsset('skwasm')
library skwasm_impl;

import 'dart:ffi';

import 'package:ui/src/engine/skwasm/skwasm_impl.dart';

final class RawVertices extends Opaque {}

typedef VerticesHandle = Pointer<RawVertices>;

@Native<
  VerticesHandle Function(
    Int vertexMode,
    Int vertexCount,
    RawPointArray positions,
    RawPointArray textureCoordinates,
    RawColorArray colors,
    Int indexCount,
    Pointer<Uint16> indices,
  )
>(symbol: 'vertices_create', isLeaf: true)
external VerticesHandle verticesCreate(
  int vertexMode,
  int vertexCount,
  RawPointArray positions,
  RawPointArray textureCoordinates,
  RawColorArray colors,
  int indexCount,
  Pointer<Uint16> indices,
);

@Native<Void Function(VerticesHandle)>(symbol: 'vertices_dispose', isLeaf: true)
external void verticesDispose(VerticesHandle handle);
