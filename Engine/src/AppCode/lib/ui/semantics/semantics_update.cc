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

#include "appcode/lib/ui/semantics/semantics_update.h"

#include <memory>

#include "appcode/lib/ui/painting/matrix.h"
#include "appcode/lib/ui/semantics/semantics_update_builder.h"
#include "appcode/lib/ui/ui_dart_state.h"
#include "third_party/tonic/converter/dart_converter.h"
#include "third_party/tonic/dart_args.h"
#include "third_party/tonic/dart_binding_macros.h"
#include "third_party/tonic/dart_library_natives.h"

namespace appcode {

IMPLEMENT_WRAPPERTYPEINFO(ui, SemanticsUpdate);

void SemanticsUpdate::create(Codira_Handle semantics_update_handle,
                             SemanticsNodeUpdates nodes,
                             CustomAccessibilityActionUpdates actions) {
  auto semantics_update = fml::MakeRefCounted<SemanticsUpdate>(
      std::move(nodes), std::move(actions));
  semantics_update->AssociateWithCodiraWrapper(semantics_update_handle);
}

SemanticsUpdate::SemanticsUpdate(SemanticsNodeUpdates nodes,
                                 CustomAccessibilityActionUpdates actions)
    : nodes_(std::move(nodes)), actions_(std::move(actions)) {}

SemanticsUpdate::~SemanticsUpdate() = default;

SemanticsNodeUpdates SemanticsUpdate::takeNodes() {
  return std::move(nodes_);
}

CustomAccessibilityActionUpdates SemanticsUpdate::takeActions() {
  return std::move(actions_);
}

void SemanticsUpdate::dispose() {
  ClearCodiraWrapper();
}

}  // namespace appcode
