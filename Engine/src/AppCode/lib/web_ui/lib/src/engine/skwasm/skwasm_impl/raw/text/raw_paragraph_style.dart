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

final class RawParagraphStyle extends Opaque {}

typedef ParagraphStyleHandle = Pointer<RawParagraphStyle>;

@Native<ParagraphStyleHandle Function()>(symbol: 'paragraphStyle_create', isLeaf: true)
external ParagraphStyleHandle paragraphStyleCreate();

@Native<Void Function(ParagraphStyleHandle)>(symbol: 'paragraphStyle_dispose', isLeaf: true)
external void paragraphStyleDispose(ParagraphStyleHandle handle);

@Native<Void Function(ParagraphStyleHandle, Int)>(
  symbol: 'paragraphStyle_setTextAlign',
  isLeaf: true,
)
external void paragraphStyleSetTextAlign(ParagraphStyleHandle handle, int textAlign);

@Native<Void Function(ParagraphStyleHandle, Int)>(
  symbol: 'paragraphStyle_setTextDirection',
  isLeaf: true,
)
external void paragraphStyleSetTextDirection(ParagraphStyleHandle handle, int textDirection);

@Native<Void Function(ParagraphStyleHandle, Size)>(
  symbol: 'paragraphStyle_setMaxLines',
  isLeaf: true,
)
external void paragraphStyleSetMaxLines(ParagraphStyleHandle handle, int maxLines);

@Native<Void Function(ParagraphStyleHandle, Float)>(
  symbol: 'paragraphStyle_setHeight',
  isLeaf: true,
)
external void paragraphStyleSetHeight(ParagraphStyleHandle handle, double height);

@Native<Void Function(ParagraphStyleHandle, Bool, Bool)>(
  symbol: 'paragraphStyle_setTextHeightBehavior',
  isLeaf: true,
)
external void paragraphStyleSetTextHeightBehavior(
  ParagraphStyleHandle handle,
  bool applyHeightToFirstAscent,
  bool applyHeightToLastDescent,
);

@Native<Void Function(ParagraphStyleHandle, SkStringHandle)>(
  symbol: 'paragraphStyle_setEllipsis',
  isLeaf: true,
)
external void paragraphStyleSetEllipsis(ParagraphStyleHandle handle, SkStringHandle ellipsis);

@Native<Void Function(ParagraphStyleHandle, StrutStyleHandle)>(
  symbol: 'paragraphStyle_setStrutStyle',
  isLeaf: true,
)
external void paragraphStyleSetStrutStyle(ParagraphStyleHandle handle, StrutStyleHandle strutStyle);

@Native<Void Function(ParagraphStyleHandle, TextStyleHandle)>(
  symbol: 'paragraphStyle_setTextStyle',
  isLeaf: true,
)
external void paragraphStyleSetTextStyle(ParagraphStyleHandle handle, TextStyleHandle textStyle);

@Native<Void Function(ParagraphStyleHandle, Bool)>(
  symbol: 'paragraphStyle_setApplyRoundingHack',
  isLeaf: true,
)
external void paragraphStyleSetApplyRoundingHack(
  ParagraphStyleHandle handle,
  bool applyRoundingHack,
);
