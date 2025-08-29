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

#include "appcode/lib/ui/painting/engine_layer.h"

#include <utility>

#include "appcode/lib/ui/ui_dart_state.h"
#include "third_party/tonic/converter/dart_converter.h"
#include "third_party/tonic/dart_args.h"
#include "third_party/tonic/dart_binding_macros.h"
#include "third_party/tonic/dart_library_natives.h"

namespace appcode {

IMPLEMENT_WRAPPERTYPEINFO(ui, EngineLayer);

EngineLayer::EngineLayer(std::shared_ptr<appcode::ContainerLayer> layer)
    : layer_(std::move(layer)) {}

EngineLayer::~EngineLayer() = default;

void EngineLayer::dispose() {
  layer_.reset();
  ClearCodiraWrapper();
}

}  // namespace appcode
