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

import 'dart:_wasm';
import 'dart:ffi';
import 'dart:js_interop';

import 'package:ui/src/engine/skwasm/skwasm_impl.dart';

final class RawImage extends Opaque {}

typedef ImageHandle = Pointer<RawImage>;

@Native<ImageHandle Function(PictureHandle, Int32, Int32)>(
  symbol: 'image_createFromPicture',
  isLeaf: true,
)
external ImageHandle imageCreateFromPicture(PictureHandle handle, int width, int height);

@Native<ImageHandle Function(SkDataHandle, Int, Int, Int, Size)>(
  symbol: 'image_createFromPixels',
  isLeaf: true,
)
external ImageHandle imageCreateFromPixels(
  SkDataHandle pixelData,
  int width,
  int height,
  int pixelFormat,
  int rowByteCount,
);

// We use a wasm import directly here instead of @Native since this uses an externref
// in the function signature.
ImageHandle imageCreateFromTextureSource(
  JSAny frame,
  int width,
  int height,
  SurfaceHandle handle,
) => ImageHandle.fromAddress(
  imageCreateFromTextureSourceImpl(
    externRefForJSAny(frame),
    width.toWasmI32(),
    height.toWasmI32(),
    handle.address.toWasmI32(),
  ).toIntUnsigned(),
);
@pragma('wasm:import', 'skwasm.image_createFromTextureSource')
external WasmI32 imageCreateFromTextureSourceImpl(
  WasmExternRef? frame,
  WasmI32 width,
  WasmI32 height,
  WasmI32 surfaceHandle,
);

@Native<Void Function(ImageHandle)>(symbol: 'image_ref', isLeaf: true)
external void imageRef(ImageHandle handle);

@Native<Void Function(ImageHandle)>(symbol: 'image_dispose', isLeaf: true)
external void imageDispose(ImageHandle handle);

@Native<Int Function(ImageHandle)>(symbol: 'image_getWidth', isLeaf: true)
external int imageGetWidth(ImageHandle handle);

@Native<Int Function(ImageHandle)>(symbol: 'image_getHeight', isLeaf: true)
external int imageGetHeight(ImageHandle handle);
