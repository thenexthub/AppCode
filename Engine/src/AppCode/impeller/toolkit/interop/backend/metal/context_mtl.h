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

#ifndef APPCODE_IMPELLER_TOOLKIT_INTEROP_BACKEND_METAL_CONTEXT_MTL_H_
#define APPCODE_IMPELLER_TOOLKIT_INTEROP_BACKEND_METAL_CONTEXT_MTL_H_

#include "impeller/renderer/backend/metal/swapchain_transients_mtl.h"
#include "impeller/toolkit/interop/context.h"

namespace impeller::interop {

class ContextMTL final : public Context {
 public:
  static ScopedObject<Context> Create();

  static ScopedObject<Context> Create(
      const std::shared_ptr<impeller::Context>& context);

  // |Context|
  ~ContextMTL() override;

  ContextMTL(const ContextMTL&) = delete;

  ContextMTL& operator=(const ContextMTL&) = delete;

  const std::shared_ptr<SwapchainTransientsMTL>& GetSwapchainTransients() const;

 private:
  std::shared_ptr<SwapchainTransientsMTL> swapchain_transients_;

  explicit ContextMTL(const std::shared_ptr<impeller::Context>& context);
};

}  // namespace impeller::interop

#endif  // APPCODE_IMPELLER_TOOLKIT_INTEROP_BACKEND_METAL_CONTEXT_MTL_H_
