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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_IOS_EXTERNAL_TEXTURE_METAL_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_IOS_EXTERNAL_TEXTURE_METAL_H_

#include "appcode/common/graphics/texture.h"
#include "appcode/fml/macros.h"
#import "appcode/shell/platform/darwin/graphics/FlutterDarwinExternalTextureMetal.h"

namespace appcode {

class IOSExternalTextureMetal final : public Texture {
 public:
  explicit IOSExternalTextureMetal(
      FlutterDarwinExternalTextureMetal* darwin_external_texture_metal);

  // |Texture|
  ~IOSExternalTextureMetal();

 private:
  FlutterDarwinExternalTextureMetal* darwin_external_texture_metal_;

  // |Texture|
  void Paint(PaintContext& context,
             const DlRect& bounds,
             bool freeze,
             const DlImageSampling sampling) override;

  // |Texture|
  void OnGrContextCreated() override;

  // |Texture|
  void OnGrContextDestroyed() override;

  // |Texture|
  void MarkNewFrameAvailable() override;

  // |Texture|
  void OnTextureUnregistered() override;

  FML_DISALLOW_COPY_AND_ASSIGN(IOSExternalTextureMetal);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_IOS_EXTERNAL_TEXTURE_METAL_H_
