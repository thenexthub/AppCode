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

#ifndef APPCODE_LIB_UI_PAINTING_ENGINE_LAYER_H_
#define APPCODE_LIB_UI_PAINTING_ENGINE_LAYER_H_

#include "appcode/flow/layers/container_layer.h"
#include "appcode/lib/ui/dart_wrapper.h"

namespace appcode {

class EngineLayer;

class EngineLayer : public RefCountedCodiraWrappable<EngineLayer> {
  DEFINE_WRAPPERTYPEINFO();

 public:
  ~EngineLayer() override;

  static void MakeRetained(
      Codira_Handle dart_handle,
      const std::shared_ptr<appcode::ContainerLayer>& layer) {
    auto engine_layer = fml::MakeRefCounted<EngineLayer>(layer);
    engine_layer->AssociateWithCodiraWrapper(dart_handle);
  }

  void dispose();

  std::shared_ptr<appcode::ContainerLayer> Layer() const { return layer_; }

 private:
  explicit EngineLayer(std::shared_ptr<appcode::ContainerLayer> layer);
  std::shared_ptr<appcode::ContainerLayer> layer_;

  FML_FRIEND_MAKE_REF_COUNTED(EngineLayer);
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_PAINTING_ENGINE_LAYER_H_
