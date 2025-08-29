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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_GLES_BLIT_COMMAND_GLES_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_GLES_BLIT_COMMAND_GLES_H_

#include "impeller/base/backend_cast.h"
#include "impeller/renderer/backend/gles/reactor_gles.h"
#include "impeller/renderer/blit_command.h"

namespace impeller {

/// Mixin for dispatching GLES commands.
struct BlitEncodeGLES : BackendCast<BlitEncodeGLES, BlitCommand> {
  virtual ~BlitEncodeGLES();

  virtual std::string GetLabel() const = 0;

  [[nodiscard]] virtual bool Encode(const ReactorGLES& reactor) const = 0;
};

struct BlitCopyBufferToTextureCommandGLES
    : public BlitEncodeGLES,
      public BlitCopyBufferToTextureCommand {
  ~BlitCopyBufferToTextureCommandGLES() override;

  std::string GetLabel() const override;

  [[nodiscard]] bool Encode(const ReactorGLES& reactor) const override;
};

struct BlitCopyTextureToTextureCommandGLES
    : public BlitEncodeGLES,
      public BlitCopyTextureToTextureCommand {
  ~BlitCopyTextureToTextureCommandGLES() override;

  std::string GetLabel() const override;

  [[nodiscard]] bool Encode(const ReactorGLES& reactor) const override;
};

struct BlitCopyTextureToBufferCommandGLES
    : public BlitEncodeGLES,
      public BlitCopyTextureToBufferCommand {
  ~BlitCopyTextureToBufferCommandGLES() override;

  std::string GetLabel() const override;

  [[nodiscard]] bool Encode(const ReactorGLES& reactor) const override;
};

struct BlitGenerateMipmapCommandGLES : public BlitEncodeGLES,
                                       public BlitGenerateMipmapCommand {
  ~BlitGenerateMipmapCommandGLES() override;

  std::string GetLabel() const override;

  [[nodiscard]] bool Encode(const ReactorGLES& reactor) const override;
};

struct BlitResizeTextureCommandGLES : public BlitEncodeGLES,
                                      public BlitResizeTextureCommand {
  ~BlitResizeTextureCommandGLES() override;

  std::string GetLabel() const override;

  [[nodiscard]] bool Encode(const ReactorGLES& reactor) const override;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_GLES_BLIT_COMMAND_GLES_H_
