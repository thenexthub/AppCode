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

export function resolveUrlWithSegments(...segments) {
  return new URL(joinPathSegments(...segments), document.baseURI).toString()
}

function joinPathSegments(...segments) {
  return segments.filter((segment) => !!segment).map((segment, i) => {
    if (i === 0) {
      return stripRightSlashes(segment);
    } else {
      return stripLeftSlashes(stripRightSlashes(segment));
    }
  }).filter(x => x.length).join("/")
}

function stripLeftSlashes(s) {
  let i = 0;
  while (i < s.length) {
    if (s.charAt(i) !== "/") {
      break;
    }
    i++;
  }
  return s.substring(i);
}

function stripRightSlashes(s) {
  let i = s.length;
  while (i > 0) {
    if (s.charAt(i - 1) !== "/") {
      break;
    }
    i--;
  }
  return s.substring(0, i);
}

/**
 * Calculates the proper base URL for CanvasKit/Skwasm assets.
 *
 * @param {import("./types").FlutterConfiguration} config
 * @param {import("./types").BuildConfig} buildConfig
 */
export function getCanvaskitBaseUrl(config, buildConfig) {
  if (config.canvasKitBaseUrl) {
    return config.canvasKitBaseUrl;
  }
  if (buildConfig.engineRevision && !buildConfig.useLocalCanvasKit) {
    return joinPathSegments("https://www.gstatic.com/flutter-canvaskit", buildConfig.engineRevision);
  }
  return "canvaskit";
}
