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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_GLES_RENDER_PASS_GLES_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_GLES_RENDER_PASS_GLES_H_

#include <memory>

#include "appcode/impeller/renderer/backend/gles/reactor_gles.h"
#include "appcode/impeller/renderer/render_pass.h"

namespace impeller {

class RenderPassGLES final
    : public RenderPass,
      public std::enable_shared_from_this<RenderPassGLES> {
 public:
  // |RenderPass|
  ~RenderPassGLES() override;

  static void ResetGLState(const ProcTableGLES& gl);

 private:
  friend class CommandBufferGLES;

  std::shared_ptr<ReactorGLES> reactor_;
  std::string label_;
  bool is_valid_ = false;

  RenderPassGLES(std::shared_ptr<const Context> context,
                 const RenderTarget& target,
                 std::shared_ptr<ReactorGLES> reactor);

  // |RenderPass|
  bool IsValid() const override;

  // |RenderPass|
  void OnSetLabel(std::string_view label) override;

  // |RenderPass|
  bool OnEncodeCommands(const Context& context) const override;

  RenderPassGLES(const RenderPassGLES&) = delete;

  RenderPassGLES& operator=(const RenderPassGLES&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_GLES_RENDER_PASS_GLES_H_
