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

#ifndef APPCODE_LIB_WEB_UI_SKWASM_SKWASM_SUPPORT_H_
#define APPCODE_LIB_WEB_UI_SKWASM_SKWASM_SUPPORT_H_

#include <emscripten/threading.h>
#include <cinttypes>
#include "surface.h"
#include "third_party/skia/include/core/SkPicture.h"

using SkwasmObject = __externref_t;

extern "C" {
extern bool skwasm_isSingleThreaded();
extern void skwasm_setAssociatedObjectOnThread(unsigned long threadId,
                                               void* pointer,
                                               SkwasmObject object);
extern SkwasmObject skwasm_getAssociatedObject(void* pointer);
extern void skwasm_disposeAssociatedObjectOnThread(unsigned long threadId,
                                                   void* pointer);
extern void skwasm_connectThread(pthread_t threadId);
extern void skwasm_dispatchRenderPictures(unsigned long threadId,
                                          Skwasm::Surface* surface,
                                          sk_sp<SkPicture>* pictures,
                                          int count,
                                          uint32_t callbackId);
extern uint32_t skwasm_createOffscreenCanvas(int width, int height);
extern void skwasm_resizeCanvas(uint32_t contextHandle, int width, int height);
extern SkwasmObject skwasm_captureImageBitmap(uint32_t contextHandle,
                                              SkwasmObject imagePromises);
extern void skwasm_postImages(Skwasm::Surface* surface,
                              SkwasmObject imageBitmaps,
                              double rasterStart,
                              uint32_t callbackId);
extern unsigned int skwasm_createGlTextureFromTextureSource(
    SkwasmObject textureSource,
    int width,
    int height);
extern void skwasm_dispatchDisposeSurface(unsigned long threadId,
                                          Skwasm::Surface* surface);
extern void skwasm_dispatchRasterizeImage(unsigned long threadId,
                                          Skwasm::Surface* surface,
                                          SkImage* image,
                                          Skwasm::ImageByteFormat format,
                                          uint32_t callbackId);
extern void skwasm_postRasterizeResult(Skwasm::Surface* surface,
                                       SkData* data,
                                       uint32_t callbackId);
}

#endif  // APPCODE_LIB_WEB_UI_SKWASM_SKWASM_SUPPORT_H_
