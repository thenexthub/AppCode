
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
//
#include "appcode/display_list/skia/dl_sk_canvas.h"
#include "appcode/fml/macros.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkCanvasVirtualEnforcer.h"
#include "third_party/skia/include/utils/SkNWayCanvas.h"
#include "third_party/skia/include/utils/SkNoDrawCanvas.h"

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_CANVAS_SPY_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_CANVAS_SPY_H_

namespace appcode {

class DidDrawCanvas;

//------------------------------------------------------------------------------
/// Facilitates spying on drawing commands to an SkCanvas.
///
/// This is used to determine whether anything was drawn into
/// a canvas so it is possible to implement optimizations that
/// are specific to empty canvases.
class CanvasSpy {
 public:
  explicit CanvasSpy(SkCanvas* target_canvas);

  //----------------------------------------------------------------------------
  /// @brief      Returns true if any non transparent content has been drawn
  /// into
  ///             the spying canvas. Note that this class does tries to detect
  ///             empty canvases but in some cases may return true even for
  ///             empty canvases (e.g when a transparent image is drawn into the
  ///             canvas).
  bool DidDrawIntoCanvas();

  //----------------------------------------------------------------------------
  /// @brief      The returned canvas delegate all operations to the target
  /// canvas
  ///             while spying on them.
  DlCanvas* GetSpyingCanvas();

  //----------------------------------------------------------------------------
  /// @brief      The underlying Skia canvas that implements the spying
  ///             (mainly for testing)
  SkCanvas* GetRawSpyingCanvas();

 private:
  std::unique_ptr<SkNWayCanvas> n_way_canvas_;
  std::unique_ptr<DidDrawCanvas> did_draw_canvas_;
  DlSkCanvasAdapter adapter_;

  FML_DISALLOW_COPY_AND_ASSIGN(CanvasSpy);
};

class DidDrawCanvas final : public SkCanvasVirtualEnforcer<SkNoDrawCanvas> {
 public:
  DidDrawCanvas(int width, int height);
  ~DidDrawCanvas() override;
  bool DidDrawIntoCanvas();

 private:
  bool did_draw_ = false;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void willSave() override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  SaveLayerStrategy getSaveLayerStrategy(const SaveLayerRec&) override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  bool onDoSaveBehind(const SkRect*) override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void willRestore() override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void didConcat44(const SkM44&) override;
  void didScale(SkScalar, SkScalar) override;
  void didTranslate(SkScalar, SkScalar) override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void onDrawDRRect(const SkRRect&, const SkRRect&, const SkPaint&) override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  virtual void onDrawTextBlob(const SkTextBlob* blob,
                              SkScalar x,
                              SkScalar y,
                              const SkPaint& paint) override;
  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  virtual void onDrawPatch(const SkPoint cubics[12],
                           const SkColor colors[4],
                           const SkPoint texCoords[4],
                           SkBlendMode,
                           const SkPaint& paint) override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void onDrawPaint(const SkPaint&) override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void onDrawBehind(const SkPaint&) override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void onDrawPoints(PointMode,
                    size_t count,
                    const SkPoint pts[],
                    const SkPaint&) override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void onDrawRect(const SkRect&, const SkPaint&) override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void onDrawRegion(const SkRegion&, const SkPaint&) override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void onDrawOval(const SkRect&, const SkPaint&) override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void onDrawArc(const SkRect&,
                 SkScalar,
                 SkScalar,
                 bool,
                 const SkPaint&) override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void onDrawRRect(const SkRRect&, const SkPaint&) override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void onDrawPath(const SkPath&, const SkPaint&) override;

#ifdef SK_SUPPORT_LEGACY_ONDRAWIMAGERECT
  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void onDrawImage(const SkImage*,
                   SkScalar left,
                   SkScalar top,
                   const SkPaint*) override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void onDrawImageRect(const SkImage*,
                       const SkRect* src,
                       const SkRect& dst,
                       const SkPaint*,
                       SrcRectConstraint) override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void onDrawImageLattice(const SkImage*,
                          const Lattice&,
                          const SkRect&,
                          const SkPaint*) override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void onDrawAtlas(const SkImage*,
                   const SkRSXform[],
                   const SkRect[],
                   const SkColor[],
                   int,
                   SkBlendMode,
                   const SkRect*,
                   const SkPaint*) override;
  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void onDrawEdgeAAImageSet(const ImageSetEntry[],
                            int count,
                            const SkPoint[],
                            const SkMatrix[],
                            const SkPaint*,
                            SrcRectConstraint) override;
#endif

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void onDrawImage2(const SkImage*,
                    SkScalar left,
                    SkScalar top,
                    const SkSamplingOptions&,
                    const SkPaint*) override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void onDrawImageRect2(const SkImage*,
                        const SkRect& src,
                        const SkRect& dst,
                        const SkSamplingOptions&,
                        const SkPaint*,
                        SrcRectConstraint) override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void onDrawImageLattice2(const SkImage*,
                           const Lattice&,
                           const SkRect&,
                           SkFilterMode,
                           const SkPaint*) override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void onDrawVerticesObject(const SkVertices*,
                            SkBlendMode,
                            const SkPaint&) override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void onDrawAtlas2(const SkImage*,
                    const SkRSXform[],
                    const SkRect[],
                    const SkColor[],
                    int,
                    SkBlendMode,
                    const SkSamplingOptions&,
                    const SkRect*,
                    const SkPaint*) override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void onDrawShadowRec(const SkPath&, const SkDrawShadowRec&) override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void onClipRect(const SkRect&, SkClipOp, ClipEdgeStyle) override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void onClipRRect(const SkRRect&, SkClipOp, ClipEdgeStyle) override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void onClipPath(const SkPath&, SkClipOp, ClipEdgeStyle) override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void onClipRegion(const SkRegion&, SkClipOp) override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void onDrawPicture(const SkPicture*,
                     const SkMatrix*,
                     const SkPaint*) override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void onDrawDrawable(SkDrawable*, const SkMatrix*) override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void onDrawAnnotation(const SkRect&, const char[], SkData*) override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void onDrawEdgeAAQuad(const SkRect&,
                        const SkPoint[4],
                        SkCanvas::QuadAAFlags,
                        const SkColor4f&,
                        SkBlendMode) override;

  // |SkCanvasVirtualEnforcer<SkNoDrawCanvas>|
  void onDrawEdgeAAImageSet2(const ImageSetEntry[],
                             int count,
                             const SkPoint[],
                             const SkMatrix[],
                             const SkSamplingOptions&,
                             const SkPaint*,
                             SrcRectConstraint) override;

  void MarkDrawIfNonTransparentPaint(const SkPaint& paint);

  FML_DISALLOW_COPY_AND_ASSIGN(DidDrawCanvas);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_CANVAS_SPY_H_
