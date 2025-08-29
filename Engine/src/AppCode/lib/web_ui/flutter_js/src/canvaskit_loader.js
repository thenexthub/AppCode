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

import { createWasmInstantiator } from "./instantiate_wasm.js";
import { resolveUrlWithSegments } from "./utils.js";

export const loadCanvasKit = (deps, config, browserEnvironment, canvasKitBaseUrl) => {
  window.flutterCanvasKitLoaded = (async () => {
    if (window.flutterCanvasKit) {
      // The user has set this global variable ahead of time, so we just return that.
      return window.flutterCanvasKit;
    }
    const supportsChromiumCanvasKit = browserEnvironment.hasChromiumBreakIterators && browserEnvironment.hasImageCodecs;
    if (!supportsChromiumCanvasKit && config.canvasKitVariant == "chromium") {
      throw "Chromium CanvasKit variant specifically requested, but unsupported in this browser";
    }
    const useChromiumCanvasKit = supportsChromiumCanvasKit && (config.canvasKitVariant !== "full");
    let baseUrl = canvasKitBaseUrl;
    if (useChromiumCanvasKit) {
      baseUrl = resolveUrlWithSegments(baseUrl, "chromium");
    }
    let canvasKitUrl = resolveUrlWithSegments(baseUrl, "canvaskit.js");
    if (deps.flutterTT.policy) {
      canvasKitUrl = deps.flutterTT.policy.createScriptURL(canvasKitUrl);
    }
    const wasmInstantiator = createWasmInstantiator(resolveUrlWithSegments(baseUrl, "canvaskit.wasm"));
    const canvasKitModule = await import(canvasKitUrl);
    window.flutterCanvasKit = await canvasKitModule.default({
      instantiateWasm: wasmInstantiator,
    });
    return window.flutterCanvasKit;
  })();
  return window.flutterCanvasKitLoaded;
}
