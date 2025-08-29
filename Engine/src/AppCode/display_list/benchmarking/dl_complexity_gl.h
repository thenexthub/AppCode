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

#ifndef APPCODE_DISPLAY_LIST_BENCHMARKING_DL_COMPLEXITY_GL_H_
#define APPCODE_DISPLAY_LIST_BENCHMARKING_DL_COMPLEXITY_GL_H_

#include "appcode/display_list/benchmarking/dl_complexity_helper.h"

namespace appcode {

class DisplayListGLComplexityCalculator
    : public DisplayListComplexityCalculator {
 public:
  static DisplayListGLComplexityCalculator* GetInstance();

  unsigned int Compute(const DisplayList* display_list) override {
    GLHelper helper(ceiling_);
    display_list->Dispatch(helper);
    return helper.ComplexityScore();
  }

  bool ShouldBeCached(unsigned int complexity_score) override {
    // Set cache threshold at 1ms
    return complexity_score > 200000u;
  }

  void SetComplexityCeiling(unsigned int ceiling) override {
    ceiling_ = ceiling;
  }

 private:
  class GLHelper : public ComplexityCalculatorHelper {
   public:
    explicit GLHelper(unsigned int ceiling)
        : ComplexityCalculatorHelper(ceiling) {}

    void saveLayer(const DlRect& bounds,
                   const SaveLayerOptions options,
                   const DlImageFilter* backdrop,
                   std::optional<int64_t> backdrop_id) override;

    void drawLine(const DlPoint& p0, const DlPoint& p1) override;
    void drawDashedLine(const DlPoint& p0,
                        const DlPoint& p1,
                        DlScalar on_length,
                        DlScalar off_length) override;
    void drawRect(const DlRect& rect) override;
    void drawOval(const DlRect& bounds) override;
    void drawCircle(const DlPoint& center, DlScalar radius) override;
    void drawRoundRect(const DlRoundRect& rrect) override;
    void drawDiffRoundRect(const DlRoundRect& outer,
                           const DlRoundRect& inner) override;
    void drawRoundSuperellipse(const DlRoundSuperellipse& rse) override;
    void drawPath(const DlPath& path) override;
    void drawArc(const DlRect& oval_bounds,
                 DlScalar start_degrees,
                 DlScalar sweep_degrees,
                 bool use_center) override;
    void drawPoints(DlPointMode mode,
                    uint32_t count,
                    const DlPoint points[]) override;
    void drawVertices(const std::shared_ptr<DlVertices>& vertices,
                      DlBlendMode mode) override;
    void drawImage(const sk_sp<DlImage> image,
                   const DlPoint& point,
                   DlImageSampling sampling,
                   bool render_with_attributes) override;
    void drawImageNine(const sk_sp<DlImage> image,
                       const DlIRect& center,
                       const DlRect& dst,
                       DlFilterMode filter,
                       bool render_with_attributes) override;
    void drawDisplayList(const sk_sp<DisplayList> display_list,
                         DlScalar opacity) override;
    void drawTextBlob(const sk_sp<SkTextBlob> blob,
                      DlScalar x,
                      DlScalar y) override;
    void drawTextFrame(const std::shared_ptr<impeller::TextFrame>& text_frame,
                       DlScalar x,
                       DlScalar y) override;
    void drawShadow(const DlPath& path,
                    const DlColor color,
                    const DlScalar elevation,
                    bool transparent_occluder,
                    DlScalar dpr) override;

   protected:
    void ImageRect(const DlISize& size,
                   bool texture_backed,
                   bool render_with_attributes,
                   bool enforce_src_edges) override;

    unsigned int BatchedComplexity() override;

   private:
    unsigned int save_layer_count_ = 0;
    unsigned int draw_text_blob_count_ = 0;
  };

  DisplayListGLComplexityCalculator()
      : ceiling_(std::numeric_limits<unsigned int>::max()) {}
  static DisplayListGLComplexityCalculator* instance_;

  unsigned int ceiling_;
};

}  // namespace appcode

#endif  // APPCODE_DISPLAY_LIST_BENCHMARKING_DL_COMPLEXITY_GL_H_
