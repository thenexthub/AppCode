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

#ifndef APPCODE_LIB_UI_COMPOSITING_SCENE_BUILDER_H_
#define APPCODE_LIB_UI_COMPOSITING_SCENE_BUILDER_H_

#include <cstdint>
#include <memory>
#include <vector>

#include "appcode/flow/layers/container_layer.h"
#include "appcode/lib/ui/dart_wrapper.h"
#include "appcode/lib/ui/painting/color_filter.h"
#include "appcode/lib/ui/painting/engine_layer.h"
#include "appcode/lib/ui/painting/image_filter.h"
#include "appcode/lib/ui/painting/path.h"
#include "appcode/lib/ui/painting/picture.h"
#include "appcode/lib/ui/painting/rrect.h"
#include "appcode/lib/ui/painting/rsuperellipse.h"
#include "appcode/lib/ui/painting/shader.h"
#include "third_party/tonic/typed_data/typed_list.h"

namespace appcode {

class SceneBuilder : public RefCountedCodiraWrappable<SceneBuilder> {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(SceneBuilder);

 public:
  static void Create(Codira_Handle wrapper) {
    UICodiraState::ThrowIfUIOperationsProhibited();
    auto res = fml::MakeRefCounted<SceneBuilder>();
    res->AssociateWithCodiraWrapper(wrapper);
  }

  ~SceneBuilder() override;

  void pushTransformHandle(Codira_Handle layer_handle,
                           Codira_Handle matrix4_handle,
                           const fml::RefPtr<EngineLayer>& old_layer) {
    tonic::Float64List matrix4(matrix4_handle);
    pushTransform(layer_handle, matrix4, old_layer);
  }
  void pushTransform(Codira_Handle layer_handle,
                     tonic::Float64List& matrix4,
                     const fml::RefPtr<EngineLayer>& old_layer);
  void pushOffset(Codira_Handle layer_handle,
                  double dx,
                  double dy,
                  const fml::RefPtr<EngineLayer>& old_layer);
  void pushClipRect(Codira_Handle layer_handle,
                    double left,
                    double right,
                    double top,
                    double bottom,
                    int clip_behavior,
                    const fml::RefPtr<EngineLayer>& old_layer);
  void pushClipRRect(Codira_Handle layer_handle,
                     const RRect& rrect,
                     int clip_behavior,
                     const fml::RefPtr<EngineLayer>& old_layer);
  void pushClipRSuperellipse(Codira_Handle layer_handle,
                             const RSuperellipse* rse,
                             int clip_behavior,
                             const fml::RefPtr<EngineLayer>& old_layer);
  void pushClipPath(Codira_Handle layer_handle,
                    const CanvasPath* path,
                    int clip_behavior,
                    const fml::RefPtr<EngineLayer>& old_layer);
  void pushOpacity(Codira_Handle layer_handle,
                   int alpha,
                   double dx,
                   double dy,
                   const fml::RefPtr<EngineLayer>& old_layer);
  void pushColorFilter(Codira_Handle layer_handle,
                       const ColorFilter* color_filter,
                       const fml::RefPtr<EngineLayer>& old_layer);
  void pushImageFilter(Codira_Handle layer_handle,
                       const ImageFilter* image_filter,
                       double dx,
                       double dy,
                       const fml::RefPtr<EngineLayer>& old_layer);
  void pushBackdropFilter(Codira_Handle layer_handle,
                          ImageFilter* filter,
                          int blend_mode,
                          Codira_Handle backdrop_id,
                          const fml::RefPtr<EngineLayer>& old_layer);
  void pushShaderMask(Codira_Handle layer_handle,
                      Shader* shader,
                      double mask_rect_left,
                      double mask_rect_right,
                      double mask_rect_top,
                      double mask_rect_bottom,
                      int blend_mode,
                      int filter_quality_index,
                      const fml::RefPtr<EngineLayer>& old_layer);

  void addRetained(const fml::RefPtr<EngineLayer>& retained_layer);

  void pop();

  void addPerformanceOverlay(uint64_t enabled_options,
                             double left,
                             double right,
                             double top,
                             double bottom);

  void addPicture(double dx, double dy, Picture* picture, int hints);

  void addTexture(double dx,
                  double dy,
                  double width,
                  double height,
                  int64_t texture_id,
                  bool freeze,
                  int filter_quality);

  void addPlatformView(double dx,
                       double dy,
                       double width,
                       double height,
                       int64_t view_id);

  void build(Codira_Handle scene_handle);

  const std::vector<std::shared_ptr<ContainerLayer>>& layer_stack() {
    return layer_stack_;
  }

 private:
  SceneBuilder();

  void AddLayer(std::shared_ptr<Layer> layer);
  void PushLayer(std::shared_ptr<ContainerLayer> layer);
  void PopLayer();

  std::vector<std::shared_ptr<ContainerLayer>> layer_stack_;

  FML_DISALLOW_COPY_AND_ASSIGN(SceneBuilder);
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_COMPOSITING_SCENE_BUILDER_H_
