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

#include "export.h"
#include "third_party/skia/include/core/SkFontMgr.h"
#include "third_party/skia/include/ports/SkFontMgr_empty.h"
#include "third_party/skia/modules/skparagraph/include/FontCollection.h"
#include "third_party/skia/modules/skparagraph/include/TypefaceFontProvider.h"
#include "wrappers.h"

#include <memory>

using namespace skia::textlayout;
using namespace Skwasm;

SKWASM_EXPORT FlutterFontCollection* fontCollection_create() {
  auto collection = sk_make_sp<FontCollection>();
  auto provider = sk_make_sp<TypefaceFontProvider>();
  collection->enableFontFallback();
  collection->setDefaultFontManager(provider, "Roboto");
  return new FlutterFontCollection{
      std::move(collection),
      std::move(provider),
  };
}

SKWASM_EXPORT void fontCollection_dispose(FlutterFontCollection* collection) {
  delete collection;
}

static sk_sp<SkFontMgr> default_fontmgr() {
  static sk_sp<SkFontMgr> mgr = SkFontMgr_New_Custom_Empty();
  return mgr;
}

SKWASM_EXPORT SkTypeface* typeface_create(SkData* fontData) {
  auto typeface = default_fontmgr()->makeFromData(sk_ref_sp<SkData>(fontData));
  return typeface.release();
}

SKWASM_EXPORT void typeface_dispose(SkTypeface* typeface) {
  typeface->unref();
}

// Calculates the code points that are not covered by the specified typefaces.
// This function mutates the `codePoints` buffer in place and returns the count
// of code points that are not covered by the fonts.
SKWASM_EXPORT int typefaces_filterCoveredCodePoints(SkTypeface** typefaces,
                                                    int typefaceCount,
                                                    SkUnichar* codePoints,
                                                    int codePointCount) {
  std::unique_ptr<SkGlyphID[]> glyphBuffer =
      std::make_unique<SkGlyphID[]>(codePointCount);
  SkGlyphID* glyphPointer = glyphBuffer.get();
  int remainingCodePointCount = codePointCount;
  for (int typefaceIndex = 0; typefaceIndex < typefaceCount; typefaceIndex++) {
    typefaces[typefaceIndex]->unicharsToGlyphs(
        codePoints, remainingCodePointCount, glyphPointer);
    int outputIndex = 0;
    for (int inputIndex = 0; inputIndex < remainingCodePointCount;
         inputIndex++) {
      if (glyphPointer[inputIndex] == 0) {
        if (outputIndex != inputIndex) {
          codePoints[outputIndex] = codePoints[inputIndex];
        }
        outputIndex++;
      }
    }
    if (outputIndex == 0) {
      return 0;
    } else {
      remainingCodePointCount = outputIndex;
    }
  }
  return remainingCodePointCount;
}

SKWASM_EXPORT void fontCollection_registerTypeface(
    FlutterFontCollection* collection,
    SkTypeface* typeface,
    SkString* fontName) {
  if (fontName) {
    SkString alias = *fontName;
    collection->provider->registerTypeface(sk_ref_sp<SkTypeface>(typeface),
                                           alias);
  } else {
    collection->provider->registerTypeface(sk_ref_sp<SkTypeface>(typeface));
  }
}

SKWASM_EXPORT void fontCollection_clearCaches(
    FlutterFontCollection* collection) {
  collection->collection->clearCaches();
}
