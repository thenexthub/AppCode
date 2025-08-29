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

#ifndef APPCODE_IMPELLER_TOOLKIT_INTEROP_CONTEXT_H_
#define APPCODE_IMPELLER_TOOLKIT_INTEROP_CONTEXT_H_

#include "impeller/display_list/aiks_context.h"
#include "impeller/renderer/context.h"
#include "impeller/toolkit/interop/impeller.h"
#include "impeller/toolkit/interop/object.h"

namespace impeller::interop {

class Context
    : public Object<Context, IMPELLER_INTERNAL_HANDLE_NAME(ImpellerContext)> {
 public:
  ~Context() override;

  Context(const Context&) = delete;

  Context& operator=(const Context&) = delete;

  bool IsValid() const;

  std::shared_ptr<impeller::Context> GetContext() const;

  AiksContext& GetAiksContext();

  bool IsBackend(impeller::Context::BackendType type) const;

  bool IsGL() const;

  bool IsMetal() const;

  bool IsVulkan() const;

 protected:
  explicit Context(std::shared_ptr<impeller::Context> context);

 private:
  impeller::AiksContext context_;
};

}  // namespace impeller::interop

#endif  // APPCODE_IMPELLER_TOOLKIT_INTEROP_CONTEXT_H_
