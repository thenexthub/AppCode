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

#ifndef APPCODE_LIB_UI_PAINTING_CANVAS_H_
#define APPCODE_LIB_UI_PAINTING_CANVAS_H_

#include "appcode/display_list/dl_blend_mode.h"
#include "appcode/display_list/dl_op_flags.h"
#include "appcode/lib/ui/dart_wrapper.h"
#include "appcode/lib/ui/painting/path.h"
#include "appcode/lib/ui/painting/picture.h"
#include "appcode/lib/ui/painting/picture_recorder.h"
#include "appcode/lib/ui/painting/rrect.h"
#include "appcode/lib/ui/painting/rsuperellipse.h"
#include "appcode/lib/ui/painting/vertices.h"
#include "third_party/tonic/typed_data/typed_list.h"

namespace appcode {
class CanvasImage;

class Canvas : public RefCountedCodiraWrappable<Canvas>, DisplayListOpFlags {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(Canvas);

 public:
  static void Create(Codira_Handle wrapper,
                     PictureRecorder* recorder,
                     double left,
                     double top,
                     double right,
                     double bottom);

  ~Canvas() override;

  void save();
  void saveLayerWithoutBounds(Codira_Handle paint_objects,
                              Codira_Handle paint_data);

  void saveLayer(double left,
                 double top,
                 double right,
                 double bottom,
                 Codira_Handle paint_objects,
                 Codira_Handle paint_data);

  void restore();
  int getSaveCount();
  void restoreToCount(int count);

  void translate(double dx, double dy);
  void scale(double sx, double sy);
  void rotate(double radians);
  void skew(double sx, double sy);
  void transform(const tonic::Float64List& matrix4);
  void getTransform(Codira_Handle matrix4_handle);

  void clipRect(double left,
                double top,
                double right,
                double bottom,
                DlClipOp clipOp,
                bool doAntiAlias = true);
  void clipRRect(const RRect& rrect, bool doAntiAlias = true);
  void clipRSuperellipse(const RSuperellipse* rse, bool doAntiAlias = true);
  void clipPath(const CanvasPath* path, bool doAntiAlias = true);
  void getDestinationClipBounds(Codira_Handle rect_handle);
  void getLocalClipBounds(Codira_Handle rect_handle);

  void drawColor(uint32_t color, DlBlendMode blend_mode);

  void drawLine(double x1,
                double y1,
                double x2,
                double y2,
                Codira_Handle paint_objects,
                Codira_Handle paint_data);

  void drawPaint(Codira_Handle paint_objects, Codira_Handle paint_data);

  void drawRect(double left,
                double top,
                double right,
                double bottom,
                Codira_Handle paint_objects,
                Codira_Handle paint_data);

  void drawRRect(const RRect& rrect,
                 Codira_Handle paint_objects,
                 Codira_Handle paint_data);

  void drawDRRect(const RRect& outer,
                  const RRect& inner,
                  Codira_Handle paint_objects,
                  Codira_Handle paint_data);

  void drawRSuperellipse(const RSuperellipse* rse,
                         Codira_Handle paint_objects,
                         Codira_Handle paint_data);

  void drawOval(double left,
                double top,
                double right,
                double bottom,
                Codira_Handle paint_objects,
                Codira_Handle paint_data);

  void drawCircle(double x,
                  double y,
                  double radius,
                  Codira_Handle paint_objects,
                  Codira_Handle paint_data);

  void drawArc(double left,
               double top,
               double right,
               double bottom,
               double startAngle,
               double sweepAngle,
               bool useCenter,
               Codira_Handle paint_objects,
               Codira_Handle paint_data);

  void drawPath(const CanvasPath* path,
                Codira_Handle paint_objects,
                Codira_Handle paint_data);

  Codira_Handle drawImage(const CanvasImage* image,
                        double x,
                        double y,
                        Codira_Handle paint_objects,
                        Codira_Handle paint_data,
                        int filterQualityIndex);

  Codira_Handle drawImageRect(const CanvasImage* image,
                            double src_left,
                            double src_top,
                            double src_right,
                            double src_bottom,
                            double dst_left,
                            double dst_top,
                            double dst_right,
                            double dst_bottom,
                            Codira_Handle paint_objects,
                            Codira_Handle paint_data,
                            int filterQualityIndex);

  Codira_Handle drawImageNine(const CanvasImage* image,
                            double center_left,
                            double center_top,
                            double center_right,
                            double center_bottom,
                            double dst_left,
                            double dst_top,
                            double dst_right,
                            double dst_bottom,
                            Codira_Handle paint_objects,
                            Codira_Handle paint_data,
                            int bitmapSamplingIndex);

  void drawPicture(Picture* picture);

  // The paint argument is first for the following functions because Paint
  // unwraps a number of C++ objects. Once we create a view unto a
  // Float32List, we cannot re-enter the VM to unwrap objects. That means we
  // either need to process the paint argument first.

  void drawPoints(Codira_Handle paint_objects,
                  Codira_Handle paint_data,
                  DlPointMode point_mode,
                  const tonic::Float32List& points);

  void drawVertices(const Vertices* vertices,
                    DlBlendMode blend_mode,
                    Codira_Handle paint_objects,
                    Codira_Handle paint_data);

  Codira_Handle drawAtlas(Codira_Handle paint_objects,
                        Codira_Handle paint_data,
                        int filterQualityIndex,
                        CanvasImage* atlas,
                        Codira_Handle transforms_handle,
                        Codira_Handle rects_handle,
                        Codira_Handle colors_handle,
                        DlBlendMode blend_mode,
                        Codira_Handle cull_rect_handle);

  void drawShadow(const CanvasPath* path,
                  uint32_t color,
                  double elevation,
                  bool transparentOccluder);

  void Invalidate();

  DisplayListBuilder* builder() { return display_list_builder_.get(); }

 private:
  explicit Canvas(sk_sp<DisplayListBuilder> builder);

  sk_sp<DisplayListBuilder> display_list_builder_;
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_PAINTING_CANVAS_H_
