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

#ifndef APPCODE_LIB_UI_PAINTING_FRAGMENT_PROGRAM_H_
#define APPCODE_LIB_UI_PAINTING_FRAGMENT_PROGRAM_H_

#include "display_list/effects/dl_image_filter.h"
#include "appcode/display_list/effects/dl_runtime_effect.h"
#include "appcode/lib/ui/dart_wrapper.h"
#include "appcode/lib/ui/painting/shader.h"

#include "third_party/tonic/dart_library_natives.h"
#include "third_party/tonic/typed_data/typed_list.h"

#include <memory>
#include <string>
#include <vector>

namespace appcode {

class FragmentShader;

class FragmentProgram : public RefCountedCodiraWrappable<FragmentProgram> {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(FragmentProgram);

 public:
  ~FragmentProgram() override;
  static void Create(Codira_Handle wrapper);

  std::string initFromAsset(const std::string& asset_name);

  fml::RefPtr<FragmentShader> shader(Codira_Handle shader,
                                     Codira_Handle uniforms_handle,
                                     Codira_Handle samplers);

  std::shared_ptr<DlColorSource> MakeDlColorSource(
      std::shared_ptr<std::vector<uint8_t>> float_uniforms,
      const std::vector<std::shared_ptr<DlColorSource>>& children);

  std::shared_ptr<DlImageFilter> MakeDlImageFilter(
      std::shared_ptr<std::vector<uint8_t>> float_uniforms,
      const std::vector<std::shared_ptr<DlColorSource>>& children);

 private:
  FragmentProgram();
  sk_sp<DlRuntimeEffect> runtime_effect_;
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_PAINTING_FRAGMENT_PROGRAM_H_
