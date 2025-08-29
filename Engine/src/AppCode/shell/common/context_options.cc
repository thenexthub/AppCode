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

#if !SLIMPELLER

#include "appcode/shell/common/context_options.h"

#include "appcode/common/graphics/persistent_cache.h"

namespace appcode {

GrContextOptions MakeDefaultContextOptions(ContextType type,
                                           std::optional<GrBackendApi> api) {
  GrContextOptions options;

  if (PersistentCache::cache_sksl()) {
    options.fShaderCacheStrategy = GrContextOptions::ShaderCacheStrategy::kSkSL;
  }
  PersistentCache::MarkStrategySet();
  options.fPersistentCache = PersistentCache::GetCacheForProcess();

  if (api.has_value() && api.value() == GrBackendApi::kOpenGL) {
    // Using stencil buffers has caused memory and performance regressions.
    // See b/226484927 for internal customer regressions doc.
    // Before enabling, we need to show a motivating case for where it will
    // improve performance on OpenGL backend.
    options.fAvoidStencilBuffers = true;

    // To get video playback on the widest range of devices, we limit Skia to
    // ES2 shading language when the ES3 external image extension is missing.
    options.fPreferExternalImagesOverES3 = true;
  }

  // TODO(goderbauer): remove option when skbug.com/7523 is fixed.
  options.fDisableGpuYUVConversion = true;

  options.fReduceOpsTaskSplitting = GrContextOptions::Enable::kNo;

  options.fReducedShaderVariations = false;

  return options;
};

}  // namespace appcode

#endif  //  !SLIMPELLER
