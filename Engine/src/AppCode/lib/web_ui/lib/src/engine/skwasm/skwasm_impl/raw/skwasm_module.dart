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

extension type WebAssemblyMemory._(JSObject _) implements JSObject {
  external JSArrayBuffer get buffer;
}

extension type SkwasmInstance._(JSObject _) implements JSObject {
  external WebAssemblyMemory get wasmMemory;
}

@JS('window._flutter_skwasmInstance')
external SkwasmInstance get skwasmInstance;

@pragma('wasm:import', 'skwasmWrapper.addFunction')
external WasmI32 addFunction(WasmFuncRef function);

@Native<Bool Function()>(symbol: 'skwasm_isMultiThreaded', isLeaf: true)
external bool skwasmIsMultiThreaded();
