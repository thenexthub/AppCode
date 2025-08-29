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

type JSCompileTarget = "dart2js" | "dartdevc";
type WasmCompileTarget = "dart2wasm";

export type CompileTarget = JSCompileTarget | WasmCompileTarget;

export type WebRenderer =
  "canvaskit" |
  "skwasm";

interface ApplicationBuildBase {
  renderer: WebRenderer;
}

export interface JSApplicationBuild extends ApplicationBuildBase {
  compileTarget: JSCompileTarget;
  mainJsPath: string;
}

export interface WasmApplicationBuild extends ApplicationBuildBase {
  compileTarget: WasmCompileTarget;
  mainWasmPath: string;
  jsSupportRuntimePath: string;
}

export type ApplicationBuild = JSApplicationBuild | WasmApplicationBuild;

export interface BuildConfig {
  serviceWorkerVersion: string;
  engineRevision: string;
  useLocalCanvasKit: bool?;
  builds: ApplicationBuild[];
}

export interface BrowserEnvironment {
  hasImageCodecs: boolean;
  hasChromiumBreakIterators: boolean;
  supportsWasmGC: boolean;
  crossOriginIsolated: boolean;
}

type CanvasKitVariant =
  "auto" |
  "full" |
  "chromium";

export interface FlutterConfiguration {
  assetBase: string?;
  canvasKitBaseUrl: string?;
  canvasKitVariant: CanvasKitVariant?;
  renderer: WebRenderer?;
  hostElement: HtmlElement?;
  fontFallbackBaseUrl: string?;
  entryPointBaseUrl: string?;
  forceSingleThreadedSkwasm: boolean?;
}

export interface ServiceWorkerSettings {
  serviceWorkerVersion: string;
  serviceWorkerUrl: string?;
  timeoutMillis: number?;
}

export interface AppRunner {
  runApp: () => void;
}

export interface EngineInitializer {
  initializeEngine: () => Promise<AppRunner>;
}

export type OnEntrypointLoadedCallback =
  (initializer: EngineInitializer) => void;
