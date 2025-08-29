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

#include "appcode/flow/testing/mock_texture.h"
#include "appcode/flow/layers/layer.h"
#include "appcode/testing/display_list_testing.h"

namespace appcode {
namespace testing {

sk_sp<DlImage> MockTexture::MakeTestTexture(int w, int h, int checker_size) {
  sk_sp<SkSurface> surface =
      SkSurfaces::Raster(SkImageInfo::MakeN32Premul(w, h));
  SkCanvas* canvas = surface->getCanvas();
  SkPaint p0, p1;
  p0.setStyle(SkPaint::kFill_Style);
  p0.setColor(SK_ColorGREEN);
  p1.setStyle(SkPaint::kFill_Style);
  p1.setColor(SK_ColorBLUE);
  p1.setAlpha(128);
  for (int y = 0; y < w; y += checker_size) {
    for (int x = 0; x < h; x += checker_size) {
      SkPaint& cellp = ((x + y) & 1) == 0 ? p0 : p1;
      canvas->drawRect(SkRect::MakeXYWH(x, y, checker_size, checker_size),
                       cellp);
    }
  }
  return DlImage::Make(surface->makeImageSnapshot());
}

MockTexture::MockTexture(int64_t textureId, const sk_sp<DlImage>& texture)
    : Texture(textureId), texture_(texture) {}

void MockTexture::Paint(PaintContext& context,
                        const DlRect& bounds,
                        bool freeze,
                        const DlImageSampling sampling) {
  // MockTexture objects that are not painted are allowed to have a null
  // texture, but when we get to this method we must have a non-null texture.
  FML_DCHECK(texture_ != nullptr);
  DlRect src = DlRect::Make(texture_->GetBounds());
  if (freeze) {
    FML_DCHECK(src.GetWidth() > 2.0f && src.GetHeight() > 2.0f);
    src = src.Expand(-1.0f, -1.0f);
  }
  context.canvas->DrawImageRect(texture_, src, bounds, sampling, context.paint);
}

}  // namespace testing
}  // namespace appcode
