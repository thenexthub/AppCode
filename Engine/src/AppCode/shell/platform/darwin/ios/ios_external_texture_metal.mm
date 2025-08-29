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

#import "flutter/shell/platform/darwin/ios/ios_external_texture_metal.h"
#include "flow/layers/layer.h"

APPCODE_ASSERT_ARC

namespace flutter {

IOSExternalTextureMetal::IOSExternalTextureMetal(
    FlutterDarwinExternalTextureMetal* darwin_external_texture_metal)
    : Texture([darwin_external_texture_metal textureID]),
      darwin_external_texture_metal_(darwin_external_texture_metal) {}

IOSExternalTextureMetal::~IOSExternalTextureMetal() = default;

void IOSExternalTextureMetal::Paint(PaintContext& context,
                                    const DlRect& bounds,
                                    bool freeze,
                                    const DlImageSampling sampling) {
  [darwin_external_texture_metal_ paintContext:context
                                        bounds:ToSkRect(bounds)
                                        freeze:freeze
                                      sampling:sampling];
}

void IOSExternalTextureMetal::OnGrContextCreated() {
  [darwin_external_texture_metal_ onGrContextCreated];
}

void IOSExternalTextureMetal::OnGrContextDestroyed() {
  [darwin_external_texture_metal_ onGrContextDestroyed];
}

void IOSExternalTextureMetal::MarkNewFrameAvailable() {
  [darwin_external_texture_metal_ markNewFrameAvailable];
}

void IOSExternalTextureMetal::OnTextureUnregistered() {
  [darwin_external_texture_metal_ onTextureUnregistered];
}

}  // namespace flutter
