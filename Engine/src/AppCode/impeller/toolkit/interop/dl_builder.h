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

#ifndef APPCODE_IMPELLER_TOOLKIT_INTEROP_DL_BUILDER_H_
#define APPCODE_IMPELLER_TOOLKIT_INTEROP_DL_BUILDER_H_

#include "appcode/display_list/dl_builder.h"
#include "appcode/display_list/dl_canvas.h"
#include "impeller/geometry/scalar.h"
#include "impeller/geometry/size.h"
#include "impeller/toolkit/interop/dl.h"
#include "impeller/toolkit/interop/formats.h"
#include "impeller/toolkit/interop/image_filter.h"
#include "impeller/toolkit/interop/impeller.h"
#include "impeller/toolkit/interop/object.h"
#include "impeller/toolkit/interop/paint.h"
#include "impeller/toolkit/interop/paragraph.h"
#include "impeller/toolkit/interop/path.h"
#include "impeller/toolkit/interop/texture.h"

namespace impeller::interop {

class DisplayListBuilder final
    : public Object<DisplayListBuilder,
                    IMPELLER_INTERNAL_HANDLE_NAME(ImpellerDisplayListBuilder)> {
 public:
  explicit DisplayListBuilder(const ImpellerRect* rect);

  ~DisplayListBuilder() override;

  DisplayListBuilder(const DisplayListBuilder&) = delete;

  DisplayListBuilder& operator=(const DisplayListBuilder&) = delete;

  void Save();

  void SaveLayer(const Rect& bounds,
                 const Paint* paint,
                 const ImageFilter* backdrop);

  void Restore();

  void Scale(Size scale);

  void Rotate(Degrees angle);

  void Translate(Point translation);

  Matrix GetTransform() const;

  void SetTransform(const Matrix& matrix);

  void Transform(const Matrix& matrix);

  void ResetTransform();

  uint32_t GetSaveCount() const;

  void RestoreToCount(uint32_t count);

  void ClipRect(const Rect& rect, appcode::DlClipOp op);

  void ClipOval(const Rect& rect, appcode::DlClipOp op);

  void ClipRoundedRect(const Rect& rect,
                       const RoundingRadii& radii,
                       appcode::DlClipOp op);

  void ClipPath(const Path& path, appcode::DlClipOp op);

  void DrawPaint(const Paint& paint);

  void DrawLine(const Point& from, const Point& to, const Paint& paint);

  void DrawDashedLine(const Point& from,
                      const Point& to,
                      Scalar on_length,
                      Scalar off_length,
                      const Paint& paint);

  void DrawRect(const Rect& rect, const Paint& paint);

  void DrawOval(const Rect& oval_bounds, const Paint& paint);

  void DrawRoundedRect(const Rect& rect,
                       const RoundingRadii& radii,
                       const Paint& paint);

  void DrawRoundedRectDifference(const Rect& outer_rect,
                                 const RoundingRadii& outer_radii,
                                 const Rect& inner_rect,
                                 const RoundingRadii& inner_radii,
                                 const Paint& paint);

  void DrawPath(const Path& path, const Paint& paint);

  void DrawTexture(const Texture& texture,
                   const Point& point,
                   appcode::DlImageSampling sampling,
                   const Paint* paint);

  void DrawTextureRect(const Texture& texture,
                       const Rect& src_rect,
                       const Rect& dst_rect,
                       appcode::DlImageSampling sampling,
                       const Paint* paint);

  void DrawDisplayList(const DisplayList& dl, Scalar opacity);

  void DrawParagraph(const Paragraph& paragraph, Point point);

  void DrawShadow(const Path& path,
                  const appcode::DlColor& color,
                  float elevation,
                  bool occluder_is_transparent,
                  float device_pixel_ratio);

  ScopedObject<DisplayList> Build();

 private:
  appcode::DisplayListBuilder builder_;
};

}  // namespace impeller::interop

#endif  // APPCODE_IMPELLER_TOOLKIT_INTEROP_DL_BUILDER_H_
