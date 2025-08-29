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

#ifndef APPCODE_FLOW_LAYERS_PERFORMANCE_OVERLAY_LAYER_H_
#define APPCODE_FLOW_LAYERS_PERFORMANCE_OVERLAY_LAYER_H_

#include <string>

#include "appcode/flow/layers/layer.h"
#include "appcode/flow/stopwatch.h"
#include "appcode/fml/macros.h"

class SkTextBlob;

namespace appcode {

const int kDisplayRasterizerStatistics = 1 << 0;
const int kVisualizeRasterizerStatistics = 1 << 1;
const int kDisplayEngineStatistics = 1 << 2;
const int kVisualizeEngineStatistics = 1 << 3;

class PerformanceOverlayLayer : public Layer {
 public:
  static SkFont MakeStatisticsFont(std::string_view font_path);

  static sk_sp<SkTextBlob> MakeStatisticsText(const Stopwatch& stopwatch,
                                              const SkFont& font,
                                              std::string_view label_prefix);

  bool IsReplacing(DiffContext* context, const Layer* layer) const override {
    return layer->as_performance_overlay_layer() != nullptr;
  }

  void Diff(DiffContext* context, const Layer* old_layer) override;

  const PerformanceOverlayLayer* as_performance_overlay_layer() const override {
    return this;
  }

  explicit PerformanceOverlayLayer(uint64_t options,
                                   const char* font_path = nullptr);

  void Preroll(PrerollContext* context) override {}
  void Paint(PaintContext& context) const override;

 private:
  int options_;
  std::string font_path_;

  FML_DISALLOW_COPY_AND_ASSIGN(PerformanceOverlayLayer);
};

}  // namespace appcode

#endif  // APPCODE_FLOW_LAYERS_PERFORMANCE_OVERLAY_LAYER_H_
