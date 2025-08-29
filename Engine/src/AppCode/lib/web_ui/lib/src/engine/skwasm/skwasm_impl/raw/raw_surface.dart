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

final class RawSurface extends Opaque {}

typedef SurfaceHandle = Pointer<RawSurface>;

final class RawRenderCallback extends Opaque {}

typedef OnRenderCallbackHandle = Pointer<RawRenderCallback>;

typedef CallbackId = int;

@Native<SurfaceHandle Function()>(symbol: 'surface_create', isLeaf: true)
external SurfaceHandle surfaceCreate();

@Native<UnsignedLong Function(SurfaceHandle)>(symbol: 'surface_getThreadId', isLeaf: true)
external int surfaceGetThreadId(SurfaceHandle handle);

@Native<Void Function(SurfaceHandle, OnRenderCallbackHandle)>(
  symbol: 'surface_setCallbackHandler',
  isLeaf: true,
)
external void surfaceSetCallbackHandler(SurfaceHandle surface, OnRenderCallbackHandle callback);

@Native<Void Function(SurfaceHandle)>(symbol: 'surface_destroy', isLeaf: true)
external void surfaceDestroy(SurfaceHandle surface);

@Native<Int32 Function(SurfaceHandle, Pointer<PictureHandle>, Int)>(
  symbol: 'surface_renderPictures',
  isLeaf: true,
)
external CallbackId surfaceRenderPictures(
  SurfaceHandle surface,
  Pointer<PictureHandle> picture,
  int count,
);

@Native<Int32 Function(SurfaceHandle, ImageHandle, Int)>(
  symbol: 'surface_rasterizeImage',
  isLeaf: true,
)
external CallbackId surfaceRasterizeImage(SurfaceHandle handle, ImageHandle image, int format);
