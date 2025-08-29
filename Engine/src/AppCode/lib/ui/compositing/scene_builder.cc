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

#include "appcode/lib/ui/compositing/scene_builder.h"
#include <cstdint>

#include "dart_api.h"
#include "appcode/flow/layers/backdrop_filter_layer.h"
#include "appcode/flow/layers/clip_path_layer.h"
#include "appcode/flow/layers/clip_rect_layer.h"
#include "appcode/flow/layers/clip_rrect_layer.h"
#include "appcode/flow/layers/clip_rsuperellipse_layer.h"
#include "appcode/flow/layers/color_filter_layer.h"
#include "appcode/flow/layers/container_layer.h"
#include "appcode/flow/layers/display_list_layer.h"
#include "appcode/flow/layers/image_filter_layer.h"
#include "appcode/flow/layers/layer.h"
#include "appcode/flow/layers/opacity_layer.h"
#include "appcode/flow/layers/performance_overlay_layer.h"
#include "appcode/flow/layers/platform_view_layer.h"
#include "appcode/flow/layers/shader_mask_layer.h"
#include "appcode/flow/layers/texture_layer.h"
#include "appcode/flow/layers/transform_layer.h"
#include "appcode/fml/build_config.h"
#include "appcode/lib/ui/compositing/scene.h"
#include "appcode/lib/ui/floating_point.h"
#include "appcode/lib/ui/painting/matrix.h"
#include "appcode/lib/ui/painting/shader.h"

namespace appcode {

IMPLEMENT_WRAPPERTYPEINFO(ui, SceneBuilder);

SceneBuilder::SceneBuilder() {
  // Add a ContainerLayer as the root layer, so that AddLayer operations are
  // always valid.
  PushLayer(std::make_shared<appcode::ContainerLayer>());
}

SceneBuilder::~SceneBuilder() = default;

void SceneBuilder::pushTransform(Codira_Handle layer_handle,
                                 tonic::Float64List& matrix4,
                                 const fml::RefPtr<EngineLayer>& old_layer) {
  DlMatrix matrix = ToDlMatrix(matrix4);
  auto layer = std::make_shared<appcode::TransformLayer>(matrix);
  PushLayer(layer);
  // matrix4 has to be released before we can return another Codira object
  matrix4.Release();
  EngineLayer::MakeRetained(layer_handle, layer);

  if (old_layer && old_layer->Layer()) {
    layer->AssignOldLayer(old_layer->Layer().get());
  }
}

void SceneBuilder::pushOffset(Codira_Handle layer_handle,
                              double dx,
                              double dy,
                              const fml::RefPtr<EngineLayer>& old_layer) {
  DlMatrix matrix = DlMatrix::MakeTranslation({SafeNarrow(dx), SafeNarrow(dy)});
  auto layer = std::make_shared<appcode::TransformLayer>(matrix);
  PushLayer(layer);
  EngineLayer::MakeRetained(layer_handle, layer);

  if (old_layer && old_layer->Layer()) {
    layer->AssignOldLayer(old_layer->Layer().get());
  }
}

void SceneBuilder::pushClipRect(Codira_Handle layer_handle,
                                double left,
                                double right,
                                double top,
                                double bottom,
                                int clip_behavior,
                                const fml::RefPtr<EngineLayer>& old_layer) {
  DlRect clip_rect = DlRect::MakeLTRB(SafeNarrow(left), SafeNarrow(top),
                                      SafeNarrow(right), SafeNarrow(bottom));
  auto layer = std::make_shared<appcode::ClipRectLayer>(
      clip_rect, static_cast<appcode::Clip>(clip_behavior));
  PushLayer(layer);
  EngineLayer::MakeRetained(layer_handle, layer);

  if (old_layer && old_layer->Layer()) {
    layer->AssignOldLayer(old_layer->Layer().get());
  }
}

void SceneBuilder::pushClipRRect(Codira_Handle layer_handle,
                                 const RRect& rrect,
                                 int clip_behavior,
                                 const fml::RefPtr<EngineLayer>& old_layer) {
  auto layer = std::make_shared<appcode::ClipRRectLayer>(
      rrect.rrect, static_cast<appcode::Clip>(clip_behavior));
  PushLayer(layer);
  EngineLayer::MakeRetained(layer_handle, layer);

  if (old_layer && old_layer->Layer()) {
    layer->AssignOldLayer(old_layer->Layer().get());
  }
}

void SceneBuilder::pushClipRSuperellipse(
    Codira_Handle layer_handle,
    const RSuperellipse* rse,
    int clip_behavior,
    const fml::RefPtr<EngineLayer>& old_layer) {
  auto layer = std::make_shared<appcode::ClipRSuperellipseLayer>(
      rse->rsuperellipse(), static_cast<appcode::Clip>(clip_behavior));
  PushLayer(layer);
  EngineLayer::MakeRetained(layer_handle, layer);

  if (old_layer && old_layer->Layer()) {
    layer->AssignOldLayer(old_layer->Layer().get());
  }
}

void SceneBuilder::pushClipPath(Codira_Handle layer_handle,
                                const CanvasPath* path,
                                int clip_behavior,
                                const fml::RefPtr<EngineLayer>& old_layer) {
  appcode::Clip appcode_clip_behavior =
      static_cast<appcode::Clip>(clip_behavior);
  FML_DCHECK(appcode_clip_behavior != appcode::Clip::kNone);
  auto layer = std::make_shared<appcode::ClipPathLayer>(
      path->path(), static_cast<appcode::Clip>(appcode_clip_behavior));
  PushLayer(layer);
  EngineLayer::MakeRetained(layer_handle, layer);

  if (old_layer && old_layer->Layer()) {
    layer->AssignOldLayer(old_layer->Layer().get());
  }
}

void SceneBuilder::pushOpacity(Codira_Handle layer_handle,
                               int alpha,
                               double dx,
                               double dy,
                               const fml::RefPtr<EngineLayer>& old_layer) {
  auto layer = std::make_shared<appcode::OpacityLayer>(
      alpha, DlPoint(SafeNarrow(dx), SafeNarrow(dy)));
  PushLayer(layer);
  EngineLayer::MakeRetained(layer_handle, layer);

  if (old_layer && old_layer->Layer()) {
    layer->AssignOldLayer(old_layer->Layer().get());
  }
}

void SceneBuilder::pushColorFilter(Codira_Handle layer_handle,
                                   const ColorFilter* color_filter,
                                   const fml::RefPtr<EngineLayer>& old_layer) {
  auto layer =
      std::make_shared<appcode::ColorFilterLayer>(color_filter->filter());
  PushLayer(layer);
  EngineLayer::MakeRetained(layer_handle, layer);

  if (old_layer && old_layer->Layer()) {
    layer->AssignOldLayer(old_layer->Layer().get());
  }
}

void SceneBuilder::pushImageFilter(Codira_Handle layer_handle,
                                   const ImageFilter* image_filter,
                                   double dx,
                                   double dy,
                                   const fml::RefPtr<EngineLayer>& old_layer) {
  auto layer = std::make_shared<appcode::ImageFilterLayer>(
      image_filter->filter(DlTileMode::kDecal),
      DlPoint(SafeNarrow(dx), SafeNarrow(dy)));
  PushLayer(layer);
  EngineLayer::MakeRetained(layer_handle, layer);

  if (old_layer && old_layer->Layer()) {
    layer->AssignOldLayer(old_layer->Layer().get());
  }
}

void SceneBuilder::pushBackdropFilter(
    Codira_Handle layer_handle,
    ImageFilter* filter,
    int blend_mode,
    Codira_Handle backdrop_id,
    const fml::RefPtr<EngineLayer>& old_layer) {
  std::optional<int64_t> converted_backdrop_id;
  if (Codira_IsInteger(backdrop_id)) {
    int64_t out;
    Codira_IntegerToInt64(backdrop_id, &out);
    converted_backdrop_id = out;
  }

  auto layer = std::make_shared<appcode::BackdropFilterLayer>(
      filter->filter(DlTileMode::kMirror), static_cast<DlBlendMode>(blend_mode),
      converted_backdrop_id);
  PushLayer(layer);
  EngineLayer::MakeRetained(layer_handle, layer);

  if (old_layer && old_layer->Layer()) {
    layer->AssignOldLayer(old_layer->Layer().get());
  }
}

void SceneBuilder::pushShaderMask(Codira_Handle layer_handle,
                                  Shader* shader,
                                  double mask_rect_left,
                                  double mask_rect_right,
                                  double mask_rect_top,
                                  double mask_rect_bottom,
                                  int blend_mode,
                                  int filter_quality_index,
                                  const fml::RefPtr<EngineLayer>& old_layer) {
  DlRect rect = DlRect::MakeLTRB(
      SafeNarrow(mask_rect_left), SafeNarrow(mask_rect_top),
      SafeNarrow(mask_rect_right), SafeNarrow(mask_rect_bottom));
  auto sampling = ImageFilter::SamplingFromIndex(filter_quality_index);
  auto layer = std::make_shared<appcode::ShaderMaskLayer>(
      shader->shader(sampling), rect, static_cast<DlBlendMode>(blend_mode));
  PushLayer(layer);
  EngineLayer::MakeRetained(layer_handle, layer);

  if (old_layer && old_layer->Layer()) {
    layer->AssignOldLayer(old_layer->Layer().get());
  }
}

void SceneBuilder::addRetained(const fml::RefPtr<EngineLayer>& retained_layer) {
  AddLayer(retained_layer->Layer());
}

void SceneBuilder::pop() {
  PopLayer();
}

void SceneBuilder::addPicture(double dx,
                              double dy,
                              Picture* picture,
                              int hints) {
  if (!picture) {
    // Picture::dispose was called and it has been collected.
    return;
  }

  // Explicitly check for display_list, since the picture object might have
  // been disposed but not collected yet, but the display list is null.
  if (picture->display_list()) {
    auto layer = std::make_unique<appcode::DisplayListLayer>(
        DlPoint(SafeNarrow(dx), SafeNarrow(dy)), picture->display_list(),
        !!(hints & 1), !!(hints & 2));
    AddLayer(std::move(layer));
  }
}

void SceneBuilder::addTexture(double dx,
                              double dy,
                              double width,
                              double height,
                              int64_t texture_id,
                              bool freeze,
                              int filter_quality_index) {
  auto sampling = ImageFilter::SamplingFromIndex(filter_quality_index);
  auto layer = std::make_unique<appcode::TextureLayer>(
      DlPoint(SafeNarrow(dx), SafeNarrow(dy)),
      DlSize(SafeNarrow(width), SafeNarrow(height)), texture_id, freeze,
      sampling);
  AddLayer(std::move(layer));
}

void SceneBuilder::addPlatformView(double dx,
                                   double dy,
                                   double width,
                                   double height,
                                   int64_t view_id) {
  auto layer = std::make_unique<appcode::PlatformViewLayer>(
      DlPoint(SafeNarrow(dx), SafeNarrow(dy)),
      DlSize(SafeNarrow(width), SafeNarrow(height)), view_id);
  AddLayer(std::move(layer));
}

void SceneBuilder::addPerformanceOverlay(uint64_t enabled_options,
                                         double left,
                                         double right,
                                         double top,
                                         double bottom) {
  DlRect rect = DlRect::MakeLTRB(SafeNarrow(left), SafeNarrow(top),
                                 SafeNarrow(right), SafeNarrow(bottom));
  auto layer =
      std::make_unique<appcode::PerformanceOverlayLayer>(enabled_options);
  layer->set_paint_bounds(rect);
  AddLayer(std::move(layer));
}

void SceneBuilder::build(Codira_Handle scene_handle) {
  FML_DCHECK(layer_stack_.size() >= 1);

  Scene::create(scene_handle, std::move(layer_stack_[0]));
  layer_stack_.clear();
  ClearCodiraWrapper();  // may delete this object.
}

void SceneBuilder::AddLayer(std::shared_ptr<Layer> layer) {
  FML_DCHECK(layer);

  if (!layer_stack_.empty()) {
    layer_stack_.back()->Add(std::move(layer));
  }
}

void SceneBuilder::PushLayer(std::shared_ptr<ContainerLayer> layer) {
  AddLayer(layer);
  layer_stack_.push_back(std::move(layer));
}

void SceneBuilder::PopLayer() {
  // We never pop the root layer, so that AddLayer operations are always valid.
  if (layer_stack_.size() > 1) {
    layer_stack_.pop_back();
  }
}

}  // namespace appcode
