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

#ifndef APPCODE_IMPELLER_TOOLKIT_INTEROP_TEXTURE_H_
#define APPCODE_IMPELLER_TOOLKIT_INTEROP_TEXTURE_H_

#include "impeller/core/texture.h"
#include "impeller/display_list/dl_image_impeller.h"
#include "impeller/toolkit/interop/context.h"
#include "impeller/toolkit/interop/impeller.h"
#include "impeller/toolkit/interop/object.h"

namespace impeller::interop {

class Texture final
    : public Object<Texture, IMPELLER_INTERNAL_HANDLE_NAME(ImpellerTexture)> {
 public:
  explicit Texture(const Context& context, const TextureDescriptor& descriptor);

  explicit Texture(impeller::Context::BackendType backend,
                   std::shared_ptr<impeller::Texture> texture);

  ~Texture() override;

  Texture(const Texture&) = delete;

  Texture& operator=(const Texture&) = delete;

  bool IsValid() const;

  bool SetContents(const uint8_t* contents, uint64_t length);

  bool SetContents(std::shared_ptr<const fml::Mapping> contents);

  sk_sp<DlImageImpeller> MakeImage() const;

  impeller::Context::BackendType GetBackendType() const;

  const std::shared_ptr<impeller::Texture>& GetTexture() const;

 private:
  impeller::Context::BackendType backend_ =
      impeller::Context::BackendType::kMetal;
  std::shared_ptr<impeller::Texture> texture_;
};

}  // namespace impeller::interop

#endif  // APPCODE_IMPELLER_TOOLKIT_INTEROP_TEXTURE_H_
