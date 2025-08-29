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

#ifndef APPCODE_DISPLAY_LIST_SKIA_DL_SK_CANVAS_H_
#define APPCODE_DISPLAY_LIST_SKIA_DL_SK_CANVAS_H_

#if !SLIMPELLER

#include "appcode/display_list/dl_canvas.h"
#include "appcode/display_list/skia/dl_sk_types.h"
#include "impeller/typographer/text_frame.h"

namespace appcode {

// -----------------------------------------------------------------------------
/// @brief      Backend implementation of |DlCanvas| for |SkCanvas|.
///
/// @see        DlCanvas
class DlSkCanvasAdapter final : public virtual DlCanvas {
 public:
  DlSkCanvasAdapter() : delegate_(nullptr) {}
  explicit DlSkCanvasAdapter(SkCanvas* canvas) : delegate_(canvas) {}
  ~DlSkCanvasAdapter() override = default;

  void set_canvas(SkCanvas* canvas);
  SkCanvas* canvas() { return delegate_; }

  DlISize GetBaseLayerDimensions() const override;
  SkImageInfo GetImageInfo() const override;

  void Save() override;
  void SaveLayer(const std::optional<DlRect>& bounds,
                 const DlPaint* paint = nullptr,
                 const DlImageFilter* backdrop = nullptr,
                 std::optional<int64_t> backdrop_id = std::nullopt) override;
  void Restore() override;
  int GetSaveCount() const override;
  void RestoreToCount(int restore_count) override;

  void Translate(DlScalar tx, DlScalar ty) override;
  void Scale(DlScalar sx, DlScalar sy) override;
  void Rotate(DlScalar degrees) override;
  void Skew(DlScalar sx, DlScalar sy) override;

  // clang-format off

  // 2x3 2D affine subset of a 4x4 transform in row major order
  void Transform2DAffine(DlScalar mxx, DlScalar mxy, DlScalar mxt,
                         DlScalar myx, DlScalar myy, DlScalar myt) override;
  // full 4x4 transform in row major order
  void TransformFullPerspective(
      DlScalar mxx, DlScalar mxy, DlScalar mxz, DlScalar mxt,
      DlScalar myx, DlScalar myy, DlScalar myz, DlScalar myt,
      DlScalar mzx, DlScalar mzy, DlScalar mzz, DlScalar mzt,
      DlScalar mwx, DlScalar mwy, DlScalar mwz, DlScalar mwt) override;
  // clang-format on
  void TransformReset() override;
  void Transform(const DlMatrix& matrix) override;
  void SetTransform(const DlMatrix& matrix) override;

  /// Returns the 4x4 full perspective transform representing all transform
  /// operations executed so far in this DisplayList within the enclosing
  /// save stack.
  DlMatrix GetMatrix() const override;

  void ClipRect(const DlRect& rect, DlClipOp clip_op, bool is_aa) override;
  void ClipOval(const DlRect& bounds, DlClipOp clip_op, bool is_aa) override;
  void ClipRoundRect(const DlRoundRect& rrect,
                     DlClipOp clip_op,
                     bool is_aa) override;
  void ClipRoundSuperellipse(const DlRoundSuperellipse& rse,
                             DlClipOp clip_op,
                             bool is_aa) override;
  void ClipPath(const DlPath& path, DlClipOp clip_op, bool is_aa) override;

  /// Conservative estimate of the bounds of all outstanding clip operations
  /// measured in the coordinate space within which this DisplayList will
  /// be rendered.
  DlRect GetDestinationClipCoverage() const override;
  /// Conservative estimate of the bounds of all outstanding clip operations
  /// transformed into the local coordinate space in which currently
  /// recorded rendering operations are interpreted.
  DlRect GetLocalClipCoverage() const override;

  /// Return true iff the supplied bounds are easily shown to be outside
  /// of the current clip bounds. This method may conservatively return
  /// false if it cannot make the determination.
  bool QuickReject(const DlRect& bounds) const override;

  void DrawPaint(const DlPaint& paint) override;
  void DrawColor(DlColor color, DlBlendMode mode) override;
  void DrawLine(const DlPoint& p0,
                const DlPoint& p1,
                const DlPaint& paint) override;
  void DrawDashedLine(const DlPoint& p0,
                      const DlPoint& p1,
                      DlScalar on_length,
                      DlScalar off_length,
                      const DlPaint& paint) override;
  void DrawRect(const DlRect& rect, const DlPaint& paint) override;
  void DrawOval(const DlRect& bounds, const DlPaint& paint) override;
  void DrawCircle(const DlPoint& center,
                  DlScalar radius,
                  const DlPaint& paint) override;
  void DrawRoundRect(const DlRoundRect& rrect, const DlPaint& paint) override;
  void DrawDiffRoundRect(const DlRoundRect& outer,
                         const DlRoundRect& inner,
                         const DlPaint& paint) override;
  void DrawRoundSuperellipse(const DlRoundSuperellipse& rse,
                             const DlPaint& paint) override;
  void DrawPath(const DlPath& path, const DlPaint& paint) override;
  void DrawArc(const DlRect& bounds,
               DlScalar start,
               DlScalar sweep,
               bool useCenter,
               const DlPaint& paint) override;
  void DrawPoints(DlPointMode mode,
                  uint32_t count,
                  const DlPoint pts[],
                  const DlPaint& paint) override;
  void DrawVertices(const std::shared_ptr<DlVertices>& vertices,
                    DlBlendMode mode,
                    const DlPaint& paint) override;
  void DrawImage(const sk_sp<DlImage>& image,
                 const DlPoint& point,
                 DlImageSampling sampling,
                 const DlPaint* paint = nullptr) override;
  void DrawImageRect(
      const sk_sp<DlImage>& image,
      const DlRect& src,
      const DlRect& dst,
      DlImageSampling sampling,
      const DlPaint* paint = nullptr,
      DlSrcRectConstraint constraint = DlSrcRectConstraint::kFast) override;
  void DrawImageNine(const sk_sp<DlImage>& image,
                     const DlIRect& center,
                     const DlRect& dst,
                     DlFilterMode filter,
                     const DlPaint* paint = nullptr) override;
  void DrawAtlas(const sk_sp<DlImage>& atlas,
                 const DlRSTransform xform[],
                 const DlRect tex[],
                 const DlColor colors[],
                 int count,
                 DlBlendMode mode,
                 DlImageSampling sampling,
                 const DlRect* cullRect,
                 const DlPaint* paint = nullptr) override;
  void DrawDisplayList(const sk_sp<DisplayList> display_list,
                       DlScalar opacity = SK_Scalar1) override;
  void DrawTextBlob(const sk_sp<SkTextBlob>& blob,
                    DlScalar x,
                    DlScalar y,
                    const DlPaint& paint) override;
  void DrawTextFrame(const std::shared_ptr<impeller::TextFrame>& text_frame,
                     DlScalar x,
                     DlScalar y,
                     const DlPaint& paint) override;
  void DrawShadow(const DlPath& path,
                  const DlColor color,
                  const DlScalar elevation,
                  bool transparent_occluder,
                  DlScalar dpr) override;

  void Flush() override;

 private:
  SkCanvas* delegate_;
};

}  // namespace appcode

#endif  //  !SLIMPELLER

#endif  // APPCODE_DISPLAY_LIST_SKIA_DL_SK_CANVAS_H_
