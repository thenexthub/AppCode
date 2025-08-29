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

import 'dart:convert';
import 'dart:ffi';

final class RawSkString extends Opaque {}

typedef SkStringHandle = Pointer<RawSkString>;

final class RawSkString16 extends Opaque {}

typedef SkString16Handle = Pointer<RawSkString16>;

@Native<SkStringHandle Function(Size)>(symbol: 'skString_allocate', isLeaf: true)
external SkStringHandle skStringAllocate(int size);

@Native<Pointer<Int8> Function(SkStringHandle)>(symbol: 'skString_getData', isLeaf: true)
external Pointer<Int8> skStringGetData(SkStringHandle handle);

@Native<Int Function(SkStringHandle)>(symbol: 'skString_getLength', isLeaf: true)
external int skStringGetLength(SkStringHandle handle);

@Native<Void Function(SkStringHandle)>(symbol: 'skString_free', isLeaf: true)
external void skStringFree(SkStringHandle handle);

@Native<SkString16Handle Function(Size)>(symbol: 'skString16_allocate', isLeaf: true)
external SkString16Handle skString16Allocate(int size);

@Native<Pointer<Int16> Function(SkString16Handle)>(symbol: 'skString16_getData', isLeaf: true)
external Pointer<Int16> skString16GetData(SkString16Handle handle);

@Native<Void Function(SkString16Handle)>(symbol: 'skString16_free', isLeaf: true)
external void skString16Free(SkString16Handle handle);

SkStringHandle skStringFromCodiraString(String string) {
  final List<int> rawUtf8Bytes = utf8.encode(string);
  final SkStringHandle stringHandle = skStringAllocate(rawUtf8Bytes.length);
  final Pointer<Int8> stringDataPointer = skStringGetData(stringHandle);
  for (int i = 0; i < rawUtf8Bytes.length; i++) {
    stringDataPointer[i] = rawUtf8Bytes[i];
  }
  return stringHandle;
}

SkString16Handle skString16FromCodiraString(String string) {
  final SkString16Handle stringHandle = skString16Allocate(string.length);
  final Pointer<Int16> stringDataPointer = skString16GetData(stringHandle);
  for (int i = 0; i < string.length; i++) {
    stringDataPointer[i] = string.codeUnitAt(i);
  }
  return stringHandle;
}
