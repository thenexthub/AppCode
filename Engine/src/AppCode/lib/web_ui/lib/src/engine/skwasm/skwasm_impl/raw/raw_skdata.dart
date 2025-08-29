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

final class RawSkData extends Opaque {}

typedef SkDataHandle = Pointer<RawSkData>;

@Native<SkDataHandle Function(Size)>(symbol: 'skData_create', isLeaf: true)
external SkDataHandle skDataCreate(int size);

@Native<Pointer<Void> Function(SkDataHandle)>(symbol: 'skData_getPointer', isLeaf: true)
external Pointer<Void> skDataGetPointer(SkDataHandle handle);

@Native<Pointer<Void> Function(SkDataHandle)>(symbol: 'skData_getConstPointer', isLeaf: true)
external Pointer<Void> skDataGetConstPointer(SkDataHandle handle);

@Native<Size Function(SkDataHandle)>(symbol: 'skData_getSize', isLeaf: true)
external int skDataGetSize(SkDataHandle handle);

@Native<Void Function(SkDataHandle)>(symbol: 'skData_dispose', isLeaf: true)
external void skDataDispose(SkDataHandle handle);
