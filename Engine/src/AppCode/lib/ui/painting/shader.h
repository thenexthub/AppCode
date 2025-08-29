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

#ifndef APPCODE_LIB_UI_PAINTING_SHADER_H_
#define APPCODE_LIB_UI_PAINTING_SHADER_H_

#include "appcode/display_list/effects/dl_color_source.h"
#include "appcode/lib/ui/ui_dart_state.h"

namespace appcode {

class Shader : public RefCountedCodiraWrappable<Shader> {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(Shader);

 public:
  ~Shader() override;

  virtual std::shared_ptr<DlColorSource> shader(DlImageSampling) = 0;

 protected:
  Shader() {}
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_PAINTING_SHADER_H_
