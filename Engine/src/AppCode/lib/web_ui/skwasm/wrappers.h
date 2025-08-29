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

#ifndef APPCODE_LIB_WEB_UI_SKWASM_WRAPPERS_H_
#define APPCODE_LIB_WEB_UI_SKWASM_WRAPPERS_H_

#include <emscripten/html5_webgl.h>
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "third_party/skia/modules/skparagraph/include/FontCollection.h"
#include "third_party/skia/modules/skparagraph/include/TypefaceFontProvider.h"

namespace Skwasm {

using SkwasmObject = __externref_t;

struct SurfaceWrapper {
  EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context;
  sk_sp<GrDirectContext> grContext;
  sk_sp<SkSurface> surface;
};

inline void makeCurrent(EMSCRIPTEN_WEBGL_CONTEXT_HANDLE handle) {
  if (!handle)
    return;

  int result = emscripten_webgl_make_context_current(handle);
  if (result != EMSCRIPTEN_RESULT_SUCCESS) {
    printf("make_context failed: %d", result);
  }
}

struct FlutterFontCollection {
  sk_sp<skia::textlayout::FontCollection> collection;
  sk_sp<skia::textlayout::TypefaceFontProvider> provider;
};

}  // namespace Skwasm

#endif  // APPCODE_LIB_WEB_UI_SKWASM_WRAPPERS_H_
