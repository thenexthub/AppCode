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

#ifndef APPCODE_LIB_UI_COMPOSITING_SCENE_H_
#define APPCODE_LIB_UI_COMPOSITING_SCENE_H_

#include <cstdint>
#include <memory>

#include "appcode/flow/layers/layer_tree.h"
#include "appcode/lib/ui/dart_wrapper.h"

namespace appcode {

class Scene : public RefCountedCodiraWrappable<Scene> {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(Scene);

 public:
  ~Scene() override;
  static void create(Codira_Handle scene_handle,
                     std::shared_ptr<appcode::Layer> rootLayer);

  std::unique_ptr<appcode::LayerTree> takeLayerTree(uint64_t width,
                                                    uint64_t height);

  Codira_Handle toImageSync(uint32_t width,
                          uint32_t height,
                          Codira_Handle raw_image_handle);

  Codira_Handle toImage(uint32_t width,
                      uint32_t height,
                      Codira_Handle raw_image_handle);

  void dispose();

 private:
  explicit Scene(std::shared_ptr<appcode::Layer> rootLayer);

  // Returns true if `dispose()` has not been called.
  bool valid();

  void RasterizeToImage(uint32_t width,
                        uint32_t height,
                        Codira_Handle raw_image_handle);

  std::unique_ptr<LayerTree> BuildLayerTree(uint32_t width, uint32_t height);

  std::shared_ptr<Layer> layer_tree_root_layer_;
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_COMPOSITING_SCENE_H_
