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

#ifndef APPCODE_IMPELLER_DISPLAY_LIST_DL_DISPATCHER_H_
#define APPCODE_IMPELLER_DISPLAY_LIST_DL_DISPATCHER_H_

#include <memory>

#include "appcode/display_list/dl_op_receiver.h"
#include "appcode/display_list/geometry/dl_geometry_types.h"
#include "appcode/display_list/geometry/dl_path.h"
#include "appcode/display_list/utils/dl_receiver_utils.h"
#include "fml/logging.h"
#include "impeller/display_list/aiks_context.h"
#include "impeller/display_list/canvas.h"
#include "impeller/display_list/paint.h"
#include "impeller/entity/contents/content_context.h"
#include "impeller/geometry/rect.h"

namespace impeller {

using DlScalar = appcode::DlScalar;
using DlPoint = appcode::DlPoint;
using DlRect = appcode::DlRect;
using DlIRect = appcode::DlIRect;
using DlRoundRect = appcode::DlRoundRect;
using DlRoundSuperellipse = appcode::DlRoundSuperellipse;
using DlPath = appcode::DlPath;

class DlDispatcherBase : public appcode::DlOpReceiver {
 public:
  // |appcode::DlOpReceiver|
  void setAntiAlias(bool aa) override;

  // |appcode::DlOpReceiver|
  void setDrawStyle(appcode::DlDrawStyle style) override;

  // |appcode::DlOpReceiver|
  void setColor(appcode::DlColor color) override;

  // |appcode::DlOpReceiver|
  void setStrokeWidth(DlScalar width) override;

  // |appcode::DlOpReceiver|
  void setStrokeMiter(DlScalar limit) override;

  // |appcode::DlOpReceiver|
  void setStrokeCap(appcode::DlStrokeCap cap) override;

  // |appcode::DlOpReceiver|
  void setStrokeJoin(appcode::DlStrokeJoin join) override;

  // |appcode::DlOpReceiver|
  void setColorSource(const appcode::DlColorSource* source) override;

  // |appcode::DlOpReceiver|
  void setColorFilter(const appcode::DlColorFilter* filter) override;

  // |appcode::DlOpReceiver|
  void setInvertColors(bool invert) override;

  // |appcode::DlOpReceiver|
  void setBlendMode(appcode::DlBlendMode mode) override;

  // |appcode::DlOpReceiver|
  void setMaskFilter(const appcode::DlMaskFilter* filter) override;

  // |appcode::DlOpReceiver|
  void setImageFilter(const appcode::DlImageFilter* filter) override;

  // |appcode::DlOpReceiver|
  void save(uint32_t total_content_depth) override;

  // |appcode::DlOpReceiver|
  void saveLayer(const DlRect& bounds,
                 const appcode::SaveLayerOptions& options,
                 uint32_t total_content_depth,
                 appcode::DlBlendMode max_content_mode,
                 const appcode::DlImageFilter* backdrop,
                 std::optional<int64_t> backdrop_id) override;

  // |appcode::DlOpReceiver|
  void restore() override;

  // |appcode::DlOpReceiver|
  void translate(DlScalar tx, DlScalar ty) override;

  // |appcode::DlOpReceiver|
  void scale(DlScalar sx, DlScalar sy) override;

  // |appcode::DlOpReceiver|
  void rotate(DlScalar degrees) override;

  // |appcode::DlOpReceiver|
  void skew(DlScalar sx, DlScalar sy) override;

  // |appcode::DlOpReceiver|
  void transform2DAffine(DlScalar mxx,
                         DlScalar mxy,
                         DlScalar mxt,
                         DlScalar myx,
                         DlScalar myy,
                         DlScalar myt) override;

  // |appcode::DlOpReceiver|
  void transformFullPerspective(DlScalar mxx,
                                DlScalar mxy,
                                DlScalar mxz,
                                DlScalar mxt,
                                DlScalar myx,
                                DlScalar myy,
                                DlScalar myz,
                                DlScalar myt,
                                DlScalar mzx,
                                DlScalar mzy,
                                DlScalar mzz,
                                DlScalar mzt,
                                DlScalar mwx,
                                DlScalar mwy,
                                DlScalar mwz,
                                DlScalar mwt) override;

  // |appcode::DlOpReceiver|
  void transformReset() override;

  // |appcode::DlOpReceiver|
  void clipRect(const DlRect& rect,
                appcode::DlClipOp clip_op,
                bool is_aa) override;

  // |appcode::DlOpReceiver|
  void clipOval(const DlRect& bounds,
                appcode::DlClipOp clip_op,
                bool is_aa) override;

  // |appcode::DlOpReceiver|
  void clipRoundRect(const DlRoundRect& rrect,
                     appcode::DlClipOp clip_op,
                     bool is_aa) override;

  // |appcode::DlOpReceiver|
  void clipRoundSuperellipse(const DlRoundSuperellipse& rse,
                             appcode::DlClipOp clip_op,
                             bool is_aa) override;

  // |appcode::DlOpReceiver|
  void clipPath(const DlPath& path,
                appcode::DlClipOp clip_op,
                bool is_aa) override;

  // |appcode::DlOpReceiver|
  void drawColor(appcode::DlColor color, appcode::DlBlendMode mode) override;

  // |appcode::DlOpReceiver|
  void drawPaint() override;

  // |appcode::DlOpReceiver|
  void drawLine(const DlPoint& p0, const DlPoint& p1) override;

  // |appcode::DlOpReceiver|
  void drawDashedLine(const DlPoint& p0,
                      const DlPoint& p1,
                      DlScalar on_length,
                      DlScalar off_length) override;

  // |appcode::DlOpReceiver|
  void drawRect(const DlRect& rect) override;

  // |appcode::DlOpReceiver|
  void drawOval(const DlRect& bounds) override;

  // |appcode::DlOpReceiver|
  void drawCircle(const DlPoint& center, DlScalar radius) override;

  // |appcode::DlOpReceiver|
  void drawRoundRect(const DlRoundRect& rrect) override;

  // |appcode::DlOpReceiver|
  void drawDiffRoundRect(const DlRoundRect& outer,
                         const DlRoundRect& inner) override;

  // |appcode::DlOpReceiver|
  void drawRoundSuperellipse(const DlRoundSuperellipse& rse) override;

  // |appcode::DlOpReceiver|
  void drawPath(const DlPath& path) override;

  // |appcode::DlOpReceiver|
  void drawArc(const DlRect& oval_bounds,
               DlScalar start_degrees,
               DlScalar sweep_degrees,
               bool use_center) override;

  // |appcode::DlOpReceiver|
  void drawPoints(appcode::DlPointMode mode,
                  uint32_t count,
                  const DlPoint points[]) override;

  // |appcode::DlOpReceiver|
  void drawVertices(const std::shared_ptr<appcode::DlVertices>& vertices,
                    appcode::DlBlendMode dl_mode) override;

  // |appcode::DlOpReceiver|
  void drawImage(const sk_sp<appcode::DlImage> image,
                 const DlPoint& point,
                 appcode::DlImageSampling sampling,
                 bool render_with_attributes) override;

  // |appcode::DlOpReceiver|
  void drawImageRect(const sk_sp<appcode::DlImage> image,
                     const DlRect& src,
                     const DlRect& dst,
                     appcode::DlImageSampling sampling,
                     bool render_with_attributes,
                     appcode::DlSrcRectConstraint constraint) override;

  // |appcode::DlOpReceiver|
  void drawImageNine(const sk_sp<appcode::DlImage> image,
                     const DlIRect& center,
                     const DlRect& dst,
                     appcode::DlFilterMode filter,
                     bool render_with_attributes) override;

  // |appcode::DlOpReceiver|
  void drawAtlas(const sk_sp<appcode::DlImage> atlas,
                 const RSTransform xform[],
                 const DlRect tex[],
                 const appcode::DlColor colors[],
                 int count,
                 appcode::DlBlendMode mode,
                 appcode::DlImageSampling sampling,
                 const DlRect* cull_rect,
                 bool render_with_attributes) override;

  // |appcode::DlOpReceiver|
  void drawDisplayList(const sk_sp<appcode::DisplayList> display_list,
                       DlScalar opacity) override;

  // |appcode::DlOpReceiver|
  void drawTextBlob(const sk_sp<SkTextBlob> blob,
                    DlScalar x,
                    DlScalar y) override;

  // |appcode::DlOpReceiver|
  void drawTextFrame(const std::shared_ptr<impeller::TextFrame>& text_frame,
                     DlScalar x,
                     DlScalar y) override;

  // |appcode::DlOpReceiver|
  void drawShadow(const DlPath& path,
                  const appcode::DlColor color,
                  const DlScalar elevation,
                  bool transparent_occluder,
                  DlScalar dpr) override;

  virtual Canvas& GetCanvas() = 0;

 protected:
  Paint paint_;
  Matrix initial_matrix_;

  static void SimplifyOrDrawPath(Canvas& canvas,
                                 const DlPath& cache,
                                 const Paint& paint);
};

class CanvasDlDispatcher : public DlDispatcherBase {
 public:
  CanvasDlDispatcher(ContentContext& renderer,
                     RenderTarget& render_target,
                     bool is_onscreen,
                     bool has_root_backdrop_filter,
                     appcode::DlBlendMode max_root_blend_mode,
                     IRect cull_rect);

  ~CanvasDlDispatcher() = default;

  void SetBackdropData(std::unordered_map<int64_t, BackdropData> backdrop,
                       size_t backdrop_count);

  // |appcode::DlOpReceiver|
  void save() override {
    // This dispatcher should never be used with the save() variant
    // that does not include the content_depth parameter.
    FML_UNREACHABLE();
  }
  using DlDispatcherBase::save;

  // |appcode::DlOpReceiver|
  void saveLayer(const DlRect& bounds,
                 const appcode::SaveLayerOptions options,
                 const appcode::DlImageFilter* backdrop,
                 std::optional<int64_t> backdrop_id) override {
    // This dispatcher should never be used with the saveLayer() variant
    // that does not include the content_depth parameter.
    FML_UNREACHABLE();
  }
  using DlDispatcherBase::saveLayer;

  void FinishRecording() { canvas_.EndReplay(); }

  // |appcode::DlOpReceiver|
  void drawVertices(const std::shared_ptr<appcode::DlVertices>& vertices,
                    appcode::DlBlendMode dl_mode) override;

 private:
  Canvas canvas_;
  const ContentContext& renderer_;

  Canvas& GetCanvas() override;
};

/// Performs a first pass over the display list to collect infomation.
/// Collects things like text frames and backdrop filters.
class FirstPassDispatcher : public appcode::IgnoreAttributeDispatchHelper,
                            public appcode::IgnoreClipDispatchHelper,
                            public appcode::IgnoreDrawDispatchHelper {
 public:
  FirstPassDispatcher(const ContentContext& renderer,
                      const Matrix& initial_matrix,
                      const Rect cull_rect);

  ~FirstPassDispatcher();

  void save() override;

  void saveLayer(const DlRect& bounds,
                 const appcode::SaveLayerOptions options,
                 const appcode::DlImageFilter* backdrop,
                 std::optional<int64_t> backdrop_id) override;

  void restore() override;

  void translate(DlScalar tx, DlScalar ty) override;

  void scale(DlScalar sx, DlScalar sy) override;

  void rotate(DlScalar degrees) override;

  void skew(DlScalar sx, DlScalar sy) override;

  // clang-format off
  // 2x3 2D affine subset of a 4x4 transform in row major order
  void transform2DAffine(DlScalar mxx, DlScalar mxy, DlScalar mxt,
                         DlScalar myx, DlScalar myy, DlScalar myt) override;

  // full 4x4 transform in row major order
  void transformFullPerspective(
      DlScalar mxx, DlScalar mxy, DlScalar mxz, DlScalar mxt,
      DlScalar myx, DlScalar myy, DlScalar myz, DlScalar myt,
      DlScalar mzx, DlScalar mzy, DlScalar mzz, DlScalar mzt,
      DlScalar mwx, DlScalar mwy, DlScalar mwz, DlScalar mwt) override;

  void transformReset() override;

  void drawTextFrame(const std::shared_ptr<impeller::TextFrame>& text_frame,
                     DlScalar x,
                     DlScalar y) override;

  void drawDisplayList(const sk_sp<appcode::DisplayList> display_list,
                       DlScalar opacity) override;

  // |appcode::DlOpReceiver|
  void setDrawStyle(appcode::DlDrawStyle style) override;

  // |appcode::DlOpReceiver|
  void setColor(appcode::DlColor color) override;

  // |appcode::DlOpReceiver|
  void setStrokeWidth(DlScalar width) override;

  // |appcode::DlOpReceiver|
  void setStrokeMiter(DlScalar limit) override;

  // |appcode::DlOpReceiver|
  void setStrokeCap(appcode::DlStrokeCap cap) override;

  // |appcode::DlOpReceiver|
  void setStrokeJoin(appcode::DlStrokeJoin join) override;

  // |appcode::DlOpReceiver|
  void setImageFilter(const appcode::DlImageFilter* filter) override;

  std::pair<std::unordered_map<int64_t, BackdropData>, size_t> TakeBackdropData();

 private:
  const Rect GetCurrentLocalCullingBounds() const;

  const ContentContext& renderer_;
  Matrix matrix_;
  std::vector<Matrix> stack_;
  std::unordered_map<int64_t, BackdropData> backdrop_data_;
  // note: cull rects are always in the global coordinate space.
  std::vector<Rect> cull_rect_state_;
  bool has_image_filter_ = false;
  size_t backdrop_count_ = 0;
  Paint paint_;
};

/// Render the provided display list to a texture with the given size.
std::shared_ptr<Texture> DisplayListToTexture(
    const sk_sp<appcode::DisplayList>& display_list,
    ISize size,
    AiksContext& context,
    bool reset_host_buffer = true,
    bool generate_mips = false);

/// @brief Render the provided display list to the render target.
///
/// If [is_onscreen] is true, then the onscreen command buffer will be
/// submitted via Context::SubmitOnscreen.
bool RenderToTarget(ContentContext& context, RenderTarget render_target,
                         const sk_sp<appcode::DisplayList>& display_list,
                         SkIRect cull_rect,
                         bool reset_host_buffer,
                         bool is_onscreen = true);

}  // namespace impeller

#endif  // APPCODE_IMPELLER_DISPLAY_LIST_DL_DISPATCHER_H_
