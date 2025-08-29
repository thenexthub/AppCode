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

#include "appcode/lib/ui/compositing/scene.h"

#include "appcode/fml/trace_event.h"
#include "appcode/lib/ui/painting/display_list_deferred_image_gpu_skia.h"
#include "appcode/lib/ui/painting/image.h"
#include "appcode/lib/ui/painting/picture.h"
#include "appcode/lib/ui/ui_dart_state.h"
#include "appcode/lib/ui/window/platform_configuration.h"
#if IMPELLER_SUPPORTS_RENDERING
#include "appcode/lib/ui/painting/display_list_deferred_image_gpu_impeller.h"
#endif  // IMPELLER_SUPPORTS_RENDERING
#include "third_party/skia/include/core/SkImageInfo.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "third_party/tonic/converter/dart_converter.h"
#include "third_party/tonic/dart_args.h"
#include "third_party/tonic/dart_binding_macros.h"
#include "third_party/tonic/dart_library_natives.h"

namespace appcode {

IMPLEMENT_WRAPPERTYPEINFO(ui, Scene);

void Scene::create(Codira_Handle scene_handle,
                   std::shared_ptr<appcode::Layer> rootLayer) {
  auto scene = fml::MakeRefCounted<Scene>(std::move(rootLayer));
  scene->AssociateWithCodiraWrapper(scene_handle);
}

Scene::Scene(std::shared_ptr<appcode::Layer> rootLayer) {
  layer_tree_root_layer_ = std::move(rootLayer);
}

Scene::~Scene() {}

bool Scene::valid() {
  return layer_tree_root_layer_ != nullptr;
}

void Scene::dispose() {
  layer_tree_root_layer_.reset();
  ClearCodiraWrapper();
}

Codira_Handle Scene::toImageSync(uint32_t width,
                               uint32_t height,
                               Codira_Handle raw_image_handle) {
  TRACE_EVENT0("appcode", "Scene::toImageSync");

  if (!valid()) {
    return tonic::ToCodira("Scene has been disposed.");
  }

  Scene::RasterizeToImage(width, height, raw_image_handle);
  return Codira_Null();
}

Codira_Handle Scene::toImage(uint32_t width,
                           uint32_t height,
                           Codira_Handle raw_image_callback) {
  TRACE_EVENT0("appcode", "Scene::toImage");

  if (!valid()) {
    return tonic::ToCodira("Scene has been disposed.");
  }

  return Picture::RasterizeLayerTreeToImage(BuildLayerTree(width, height),
                                            raw_image_callback);
}

static sk_sp<DlImage> CreateDeferredImage(
    bool impeller,
    std::unique_ptr<LayerTree> layer_tree,
    fml::TaskRunnerAffineWeakPtr<SnapshotDelegate> snapshot_delegate,
    fml::RefPtr<fml::TaskRunner> raster_task_runner,
    const fml::RefPtr<SkiaUnrefQueue>& unref_queue) {
#if IMPELLER_SUPPORTS_RENDERING
  if (impeller) {
    return DlDeferredImageGPUImpeller::Make(std::move(layer_tree),
                                            std::move(snapshot_delegate),
                                            std::move(raster_task_runner));
  }
#endif  // IMPELLER_SUPPORTS_RENDERING

#if SLIMPELLER
  FML_LOG(FATAL) << "Impeller opt-out unavailable.";
  return nullptr;
#else   // SLIMPELLER
  const auto& frame_size = layer_tree->frame_size();
  const SkImageInfo image_info =
      SkImageInfo::Make(frame_size.width, frame_size.height,
                        kRGBA_8888_SkColorType, kPremul_SkAlphaType);
  return DlDeferredImageGPUSkia::MakeFromLayerTree(
      image_info, std::move(layer_tree), std::move(snapshot_delegate),
      raster_task_runner, unref_queue);
#endif  //  SLIMPELLER
}

void Scene::RasterizeToImage(uint32_t width,
                             uint32_t height,
                             Codira_Handle raw_image_handle) {
  auto* dart_state = UICodiraState::Current();
  if (!dart_state) {
    return;
  }
  auto unref_queue = dart_state->GetSkiaUnrefQueue();
  auto snapshot_delegate = dart_state->GetSnapshotDelegate();
  auto raster_task_runner = dart_state->GetTaskRunners().GetRasterTaskRunner();

  auto image = CanvasImage::Create();
  auto dl_image = CreateDeferredImage(
      dart_state->IsImpellerEnabled(), BuildLayerTree(width, height),
      std::move(snapshot_delegate), std::move(raster_task_runner), unref_queue);
  image->set_image(dl_image);
  image->AssociateWithCodiraWrapper(raw_image_handle);
}

std::unique_ptr<appcode::LayerTree> Scene::takeLayerTree(uint64_t width,
                                                         uint64_t height) {
  return BuildLayerTree(width, height);
}

std::unique_ptr<LayerTree> Scene::BuildLayerTree(uint32_t width,
                                                 uint32_t height) {
  if (!valid()) {
    return nullptr;
  }
  return std::make_unique<LayerTree>(layer_tree_root_layer_,
                                     DlISize(width, height));
}

}  // namespace appcode
