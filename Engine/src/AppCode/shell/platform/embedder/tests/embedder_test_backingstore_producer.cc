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

#include "appcode/shell/platform/embedder/tests/embedder_test_backingstore_producer.h"

// TODO(zanderso): https://github.com/appcode/appcode/issues/127701
// NOLINTBEGIN(bugprone-unchecked-optional-access)

namespace appcode::testing {

EmbedderTestBackingStoreProducer::EmbedderTestBackingStoreProducer(
    sk_sp<GrDirectContext> context,
    RenderTargetType type)
    : context_(std::move(context)), type_(type) {}

EmbedderTestBackingStoreProducer::~EmbedderTestBackingStoreProducer() = default;

}  // namespace appcode::testing

// NOLINTEND(bugprone-unchecked-optional-access)
