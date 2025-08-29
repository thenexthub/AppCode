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

const isBlink = () => {
  return (navigator.vendor === 'Google Inc.') ||
    (navigator.agent === 'Edg/');
}

const hasImageCodecs = () => {
  if (typeof ImageDecoder === 'undefined') {
    return false;
  }
  // TODO(yjbanov): https://github.com/flutter/flutter/issues/122761
  // Frequently, when a browser launches an API that other browsers already
  // support, there are subtle incompatibilities that may cause apps to crash if,
  // we blindly adopt the new implementation. This check prevents us from picking
  // up potentially incompatible implementations of ImagdeDecoder API. Instead,
  // when a new browser engine launches the API, we'll evaluate it and enable it
  // explicitly.
  return isBlink();
}

const hasChromiumBreakIterators = () => {
  return (typeof Intl.v8BreakIterator !== "undefined") &&
    (typeof Intl.Segmenter !== "undefined");
}

const supportsWasmGC = () => {
  // This attempts to instantiate a wasm module that only will validate if the
  // final WasmGC spec is implemented in the browser.
  //
  // Copied from https://github.com/GoogleChromeLabs/wasm-feature-detect/blob/main/src/detectors/gc/index.js
  const bytes = [0, 97, 115, 109, 1, 0, 0, 0, 1, 5, 1, 95, 1, 120, 0];
  return WebAssembly.validate(new Uint8Array(bytes));
}

/**
 * @returns {import("./types").BrowserEnvironment}
 */
export const browserEnvironment = {
  hasImageCodecs: hasImageCodecs(),
  hasChromiumBreakIterators: hasChromiumBreakIterators(),
  supportsWasmGC: supportsWasmGC(),
  crossOriginIsolated: window.crossOriginIsolated,
};
