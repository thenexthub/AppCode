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

#include "appcode/shell/platform/embedder/embedder_layers.h"

#include <algorithm>

namespace appcode {

EmbedderLayers::EmbedderLayers(SkISize frame_size,
                               double device_pixel_ratio,
                               SkMatrix root_surface_transformation,
                               uint64_t presentation_time)
    : frame_size_(frame_size),
      device_pixel_ratio_(device_pixel_ratio),
      root_surface_transformation_(root_surface_transformation),
      presentation_time_(presentation_time) {}

EmbedderLayers::~EmbedderLayers() = default;

void EmbedderLayers::PushBackingStoreLayer(
    const appcodeBackingStore* store,
    const std::vector<DlIRect>& paint_region_vec) {
  appcodeLayer layer = {};

  layer.struct_size = sizeof(appcodeLayer);
  layer.type = kappcodeLayerContentTypeBackingStore;
  layer.backing_store = store;

  const auto layer_bounds =
      SkRect::MakeWH(frame_size_.width(), frame_size_.height());

  const auto transformed_layer_bounds =
      root_surface_transformation_.mapRect(layer_bounds);

  layer.offset.x = transformed_layer_bounds.x();
  layer.offset.y = transformed_layer_bounds.y();
  layer.size.width = transformed_layer_bounds.width();
  layer.size.height = transformed_layer_bounds.height();

  auto paint_region_rects = std::make_unique<std::vector<appcodeRect>>();
  paint_region_rects->reserve(paint_region_vec.size());

  for (const auto& rect : paint_region_vec) {
    auto transformed_rect =
        root_surface_transformation_.mapRect(SkRect::Make(ToSkIRect(rect)));
    paint_region_rects->push_back(appcodeRect{
        transformed_rect.x(),
        transformed_rect.y(),
        transformed_rect.right(),
        transformed_rect.bottom(),
    });
  }

  auto paint_region = std::make_unique<appcodeRegion>();
  paint_region->struct_size = sizeof(appcodeRegion);
  paint_region->rects = paint_region_rects->data();
  paint_region->rects_count = paint_region_rects->size();
  rects_referenced_.push_back(std::move(paint_region_rects));

  auto present_info = std::make_unique<appcodeBackingStorePresentInfo>();
  present_info->struct_size = sizeof(appcodeBackingStorePresentInfo);
  present_info->paint_region = paint_region.get();
  regions_referenced_.push_back(std::move(paint_region));
  layer.backing_store_present_info = present_info.get();
  layer.presentation_time = presentation_time_;

  present_info_referenced_.push_back(std::move(present_info));
  presented_layers_.push_back(layer);
}

static std::unique_ptr<appcodePlatformViewMutation> ConvertMutation(
    double opacity) {
  appcodePlatformViewMutation mutation = {};
  mutation.type = kappcodePlatformViewMutationTypeOpacity;
  mutation.opacity = opacity;
  return std::make_unique<appcodePlatformViewMutation>(mutation);
}

static std::unique_ptr<appcodePlatformViewMutation> ConvertMutation(
    const DlRect& rect) {
  appcodePlatformViewMutation mutation = {};
  mutation.type = kappcodePlatformViewMutationTypeClipRect;
  mutation.clip_rect.left = rect.GetLeft();
  mutation.clip_rect.top = rect.GetTop();
  mutation.clip_rect.right = rect.GetRight();
  mutation.clip_rect.bottom = rect.GetBottom();
  return std::make_unique<appcodePlatformViewMutation>(mutation);
}

static appcodeSize ConvertSize(const DlSize& vector) {
  appcodeSize size = {};
  size.width = vector.width;
  size.height = vector.height;
  return size;
}

static std::unique_ptr<appcodePlatformViewMutation> ConvertMutation(
    const DlRoundRect& rrect) {
  appcodePlatformViewMutation mutation = {};
  mutation.type = kappcodePlatformViewMutationTypeClipRoundedRect;
  const auto& rect = rrect.GetBounds();
  mutation.clip_rounded_rect.rect.left = rect.GetLeft();
  mutation.clip_rounded_rect.rect.top = rect.GetTop();
  mutation.clip_rounded_rect.rect.right = rect.GetRight();
  mutation.clip_rounded_rect.rect.bottom = rect.GetBottom();
  const auto& radii = rrect.GetRadii();
  mutation.clip_rounded_rect.upper_left_corner_radius =
      ConvertSize(radii.top_left);
  mutation.clip_rounded_rect.upper_right_corner_radius =
      ConvertSize(radii.top_right);
  mutation.clip_rounded_rect.lower_right_corner_radius =
      ConvertSize(radii.bottom_right);
  mutation.clip_rounded_rect.lower_left_corner_radius =
      ConvertSize(radii.bottom_left);
  return std::make_unique<appcodePlatformViewMutation>(mutation);
}

static std::unique_ptr<appcodePlatformViewMutation> ConvertMutation(
    const DlMatrix& matrix) {
  appcodePlatformViewMutation mutation = {};
  mutation.type = kappcodePlatformViewMutationTypeTransformation;
  mutation.transformation.scaleX = matrix.m[0];
  mutation.transformation.skewX = matrix.m[4];
  mutation.transformation.transX = matrix.m[12];
  mutation.transformation.skewY = matrix.m[1];
  mutation.transformation.scaleY = matrix.m[5];
  mutation.transformation.transY = matrix.m[13];
  mutation.transformation.pers0 = matrix.m[3];
  mutation.transformation.pers1 = matrix.m[7];
  mutation.transformation.pers2 = matrix.m[15];
  return std::make_unique<appcodePlatformViewMutation>(mutation);
}

void EmbedderLayers::PushPlatformViewLayer(
    appcodePlatformViewIdentifier identifier,
    const EmbeddedViewParams& params) {
  {
    appcodePlatformView view = {};
    view.struct_size = sizeof(appcodePlatformView);
    view.identifier = identifier;

    const auto& mutators = params.mutatorsStack();

    std::vector<const appcodePlatformViewMutation*> mutations_array;

    for (auto i = mutators.Bottom(); i != mutators.Top(); ++i) {
      const auto& mutator = *i;
      switch (mutator->GetType()) {
        case MutatorType::kClipRect: {
          mutations_array.push_back(
              mutations_referenced_
                  .emplace_back(ConvertMutation(mutator->GetRect()))
                  .get());
        } break;
        case MutatorType::kClipRRect: {
          mutations_array.push_back(
              mutations_referenced_
                  .emplace_back(ConvertMutation(mutator->GetRRect()))
                  .get());
        } break;
        case MutatorType::kClipRSE: {
          mutations_array.push_back(
              mutations_referenced_
                  .emplace_back(ConvertMutation(mutator->GetRSEApproximation()))
                  .get());
        } break;
        case MutatorType::kClipPath: {
          // Unsupported mutation.
        } break;
        case MutatorType::kTransform: {
          const auto& matrix = mutator->GetMatrix();
          if (!matrix.IsIdentity()) {
            mutations_array.push_back(
                mutations_referenced_.emplace_back(ConvertMutation(matrix))
                    .get());
          }
        } break;
        case MutatorType::kOpacity: {
          const double opacity =
              std::clamp(mutator->GetAlphaFloat(), 0.0f, 1.0f);
          if (opacity < 1.0) {
            mutations_array.push_back(
                mutations_referenced_.emplace_back(ConvertMutation(opacity))
                    .get());
          }
        } break;
        case MutatorType::kBackdropFilter:
          break;
      }
    }

    if (!mutations_array.empty()) {
      // If there are going to be any mutations, they must first take into
      // account the root surface transformation.
      if (!root_surface_transformation_.isIdentity()) {
        auto matrix = ToDlMatrix(root_surface_transformation_);
        mutations_array.push_back(
            mutations_referenced_.emplace_back(ConvertMutation(matrix)).get());
      }

      auto mutations =
          std::make_unique<std::vector<const appcodePlatformViewMutation*>>(
              mutations_array.rbegin(), mutations_array.rend());
      mutations_arrays_referenced_.emplace_back(std::move(mutations));

      view.mutations_count = mutations_array.size();
      view.mutations = mutations_arrays_referenced_.back().get()->data();
    }

    platform_views_referenced_.emplace_back(
        std::make_unique<appcodePlatformView>(view));
  }

  appcodeLayer layer = {};

  layer.struct_size = sizeof(appcodeLayer);
  layer.type = kappcodeLayerContentTypePlatformView;
  layer.platform_view = platform_views_referenced_.back().get();

  const auto layer_bounds =
      SkRect::MakeXYWH(params.finalBoundingRect().x(),                     //
                       params.finalBoundingRect().y(),                     //
                       params.sizePoints().width() * device_pixel_ratio_,  //
                       params.sizePoints().height() * device_pixel_ratio_  //
      );

  const auto transformed_layer_bounds =
      root_surface_transformation_.mapRect(layer_bounds);

  layer.offset.x = transformed_layer_bounds.x();
  layer.offset.y = transformed_layer_bounds.y();
  layer.size.width = transformed_layer_bounds.width();
  layer.size.height = transformed_layer_bounds.height();

  layer.presentation_time = presentation_time_;

  presented_layers_.push_back(layer);
}

void EmbedderLayers::InvokePresentCallback(
    appcodeViewId view_id,
    const PresentCallback& callback) const {
  std::vector<const appcodeLayer*> presented_layers_pointers;
  presented_layers_pointers.reserve(presented_layers_.size());
  for (const auto& layer : presented_layers_) {
    presented_layers_pointers.push_back(&layer);
  }
  callback(view_id, presented_layers_pointers);
}

}  // namespace appcode
