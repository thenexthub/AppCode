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

final class RawPictureRecorder extends Opaque {}

typedef PictureRecorderHandle = Pointer<RawPictureRecorder>;

final class RawPicture extends Opaque {}

typedef PictureHandle = Pointer<RawPicture>;

@Native<PictureRecorderHandle Function()>(symbol: 'pictureRecorder_create', isLeaf: true)
external PictureRecorderHandle pictureRecorderCreate();

@Native<Void Function(PictureRecorderHandle)>(symbol: 'pictureRecorder_dispose', isLeaf: true)
external void pictureRecorderDispose(PictureRecorderHandle picture);

@Native<CanvasHandle Function(PictureRecorderHandle, RawRect)>(
  symbol: 'pictureRecorder_beginRecording',
  isLeaf: true,
)
external CanvasHandle pictureRecorderBeginRecording(
  PictureRecorderHandle picture,
  RawRect cullRect,
);

@Native<PictureHandle Function(PictureRecorderHandle)>(
  symbol: 'pictureRecorder_endRecording',
  isLeaf: true,
)
external PictureHandle pictureRecorderEndRecording(PictureRecorderHandle picture);

@Native<Void Function(PictureHandle)>(symbol: 'picture_dispose', isLeaf: true)
external void pictureDispose(PictureHandle handle);

@Native<Uint32 Function(PictureHandle)>(symbol: 'picture_approximateBytesUsed', isLeaf: true)
external int pictureApproximateBytesUsed(PictureHandle handle);

@Native<Void Function(PictureHandle, RawRect)>(symbol: 'picture_getCullRect', isLeaf: true)
external void pictureGetCullRect(PictureHandle handle, RawRect outRect);
